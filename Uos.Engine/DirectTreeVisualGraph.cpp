#include "StdAfx.h"
#include "DirectTreeVisualGraph.h"
#include "DirectMesh.h"
#include "RenderLayer.h"

using namespace uos;

CDirectTreeVisualGraph::CDirectTreeVisualGraph(CEngineLevel * l, CDirectPipelineFactory * pf, const CString & name) : CTreeVisualGraph(l, pf, name)
{
	auto sh = new CShader(L"StencilClipping");
	auto geometry = sh->AddConstantBuffer(L"Geometry");
	sh->AddConstant(geometry, L"WVP", L"float4x4", EPipelineStage::Vertex);
	sh->AddVertexInput(L"Position", L"float3", L"POSITION");
	sh->AddVertexOutput(L"PositionWVP", L"float4", L"SV_POSITION");
	sh->VertexProgram = L"output.PositionWVP = mul(float4(input.Position, 1.0f), WVP);";
	sh->PixelProgram = L"output.Color = float4(1,1,1,1);";

	ClippingPipeline = pf->GetPipeline(sh);
	
	ClippingBuffer = new CDirectConstantBuffer();
	ClippingBuffer->Bind(sh, L"Geometry");
	ClippingWVPSlot	= ClippingBuffer->FindSlot(L"WVP");

	sh->Free();
}
	
CDirectTreeVisualGraph::~CDirectTreeVisualGraph()
{
	delete ClippingBuffer;
	ClippingPipeline->Free();

	for(auto & i : DxNoClipping)
		i.second->Release();

	for(auto & i : DxApplyClipping)
		i.second->Release();

	for(auto & i : DxRootIndex)
		i.second->Release();

	for(auto & i : DxIncrementIndex)
		i.second->Release();

	for(auto & i : DxRasterizerState)
		i.second->Release();

	for(auto & i : DxBlendStateNormal)
		i.second->Release();

	for(auto & i : DxBlendStateAlpha)
		i.second->Release();

	for(auto & i : DxClipperBlending)
		i.second->Release();
}

void CDirectTreeVisualGraph::Render(CRenderTarget * t, CViewport * vp, CVisualSpace * s)
{
	auto & nodes = Spaces(s);
	//auto t = st->As<CDirectScreenTarget>();
	auto d = t->As<CScreenRenderTarget>()->Device;

	if(DxRasterizerState[d] == null)
	{
		// TODO: per node/material/mesh
		D3D11_RASTERIZER_DESC rd = {};
		rd.DepthClipEnable = true;
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_BACK;
		rd.FrontCounterClockwise = false; 
		rd.MultisampleEnable = false;
		Verify(d->DxDevice->CreateRasterizerState(&rd, &DxRasterizerState[d]));
	}

	if(DxBlendStateNormal[d] == null)
	{
		D3D11_BLEND_DESC bd = {};

		bd.RenderTarget[0].BlendEnable =  false;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		Verify(d->DxDevice->CreateBlendState(&bd, &DxBlendStateNormal[d]));
	}

	if(DxBlendStateAlpha[d] == null)
	{
		D3D11_BLEND_DESC bd = {};

		for(int i=0; i < sizeof(bd.RenderTarget)/sizeof(bd.RenderTarget[0]); i++)
		{
			bd.RenderTarget[i].BlendEnable =  true;
			bd.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}

		Verify(d->DxDevice->CreateBlendState(&bd, &DxBlendStateAlpha[d]));
	}

	d->DxContext->RSSetState(DxRasterizerState[d]);

	for(auto i : nodes)
	{
		AssignPipelines(s, t, i, i);
	}


	int sref = 0;
	nodes.Do([](auto i){ i->Do([](auto j){ j->StencilIndex = 0; }); });
	
	for(auto i : nodes)
	{
		Render(s, t, vp, null, null, i, i, sref, false);
	}

	sref = 0;
	nodes.Do([](auto i){ i->Do([](auto j){ j->StencilIndex = 0; }); });

	for(auto i : nodes)
	{
		Render(s, t, vp, null, null, i, i, sref, true);
	}
}

void CDirectTreeVisualGraph::AssignPipelines(CVisualSpace * space, CRenderTarget * target, CVisual * root, CVisual * v)
{
/*
	if(!Belongs(space, root, v))
	{
		return;
	}

*/
	if(v->IsReady() && (!v->Pipeline || v->ShaderChanged))
	{
		AssignPipeline(v, Pipelines);
	}

	for(auto i : v->Nodes)
	{
		AssignPipelines(space, target, root, i);
	}
}

void CDirectTreeVisualGraph::Render(CVisualSpace * s, CRenderTarget * t, CViewport * vp, CCamera * cam, CDirectPipeline * pl, CVisual * root, CVisual * v, int & sref, bool alpha)
{
	auto dt = t->As<CScreenRenderTarget>();
	auto d = dt->Device;

	if(v->Parent == null)
	{	
		sref = 0;
		d->DxContext->ClearDepthStencilView(dt->DepthStencilView, D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	if(s->View)
	{
		cam = s->View->GetCamera(vp);
	} 

	if(cam)
	{
		if(v->IsReady() && v->Material->AlphaBlending == alpha)
			RenderVisual(d, s, v, cam, sref);
	
		if(v->ClippingMesh && v->ClippingMesh->IsReady())
			RenderClipping(d, s, v, cam, sref);
	}

	for(auto i : v->Nodes)
		Render(s, t, vp, cam, pl, root, i, sref, alpha);
}

void CDirectTreeVisualGraph::RenderVisual(CDirectDevice * d, CVisualSpace * s, CVisual * v, CCamera * c, int & sref)
{
	#ifdef _DEBUG
		v->_SSPosition = c->ProjectVertex((v->FinalMatrix * s->Matrix).GetPosition());
	#endif
		
			
	if(sref > 0 && v->GetActualClipping() == EClipping::Apply)
	{
		if(DxApplyClipping[d] == null)
		{
			D3D11_DEPTH_STENCIL_DESC desc;

			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			desc.StencilEnable = true;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;

			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_EQUAL;

			desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			Verify(d->DxDevice->CreateDepthStencilState(&desc, &DxApplyClipping[d]));
		}

		auto c = v->GetActualClipper();
		d->DxContext->OMSetDepthStencilState(DxApplyClipping[d], c->StencilIndex);
	}
	else
	{
		if(DxNoClipping[d] == null)
		{
			D3D11_DEPTH_STENCIL_DESC desc = {};

			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;

			desc.StencilEnable = false;

			Verify(d->DxDevice->CreateDepthStencilState(&desc, &DxNoClipping[d]));
		}

		d->DxContext->OMSetDepthStencilState(DxNoClipping[d], -1);
	}

	d->DxContext->OMSetBlendState(v->Material->AlphaBlending ? DxBlendStateAlpha[d] : DxBlendStateNormal[d], null, 0xffffffff);
	
	v->Pipeline->Apply(d);
	v->Apply(d, s->Matrix, c);
	v->Material->Apply(d);
	((CDirectMesh *)v->Mesh->Realization)->Draw(d);
}
	
void CDirectTreeVisualGraph::RenderClipping(CDirectDevice * d, CVisualSpace * s, CVisual * v, CCamera * c, int & index)
{
	if(index == 0 || v->GetActualClipping() == EClipping::No)
	{
		if(DxRootIndex[d] == null)
		{
			D3D11_DEPTH_STENCIL_DESC desc;

			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

			desc.StencilEnable = true;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;

			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_REPLACE;
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			Verify(d->DxDevice->CreateDepthStencilState(&desc, &DxRootIndex[d]));
		}

		v->StencilIndex = ++index;
		d->DxContext->OMSetDepthStencilState(DxRootIndex[d], index);
	}
	else
	{
		if(DxIncrementIndex[d] == null)
		{
			D3D11_DEPTH_STENCIL_DESC desc;

			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

			desc.StencilEnable = true;
			desc.StencilReadMask = 0xFF;
			desc.StencilWriteMask = 0xFF;

			desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_INCR;
			desc.FrontFace.StencilFunc			= D3D11_COMPARISON_EQUAL;

			desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

			Verify(d->DxDevice->CreateDepthStencilState(&desc, &DxIncrementIndex[d]));
		}

		d->DxContext->OMSetDepthStencilState(DxIncrementIndex[d], index);
		v->StencilIndex = ++index;
	}

	if(DxClipperBlending[d] == null)
	{
		D3D11_BLEND_DESC bd = {};

		bd.RenderTarget[0].BlendEnable =  false;
		bd.RenderTarget[0].RenderTargetWriteMask = 0;

		Verify(d->DxDevice->CreateBlendState(&bd, &DxClipperBlending[d]));
	}

	d->DxContext->OMSetBlendState(DxClipperBlending[d], null, 0xffffffff);

	if(index == 255)
	{
		throw CException(HERE, L"SI = 255");
	}
	
	ClippingPipeline->Apply(d);
	ClippingBuffer->SetValue(ClippingWVPSlot, v->FinalMatrix * s->Matrix * c->GetViewProjectionMatrix());
	ClippingBuffer->Apply(d);
	((CDirectMesh *)v->ClippingMesh->Realization)->Draw(d);
}

//void CTreeVisualGraph::UnrenderClipping(CVisual * v, CViewport * vp, CRenderer * r, int & index)
//{
//	CProjector * pj = v->GetActualView()->GetProjector(vp);
//
//	if(pj)
//	{
//					
//		vp->Target->Driver->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
//		vp->Target->Driver->SetRenderState(D3DRS_STENCILREF, index);
//		vp->Target->Driver->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
//		index--;
//
//		vp->Target->Driver->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0);
//		//vp->Target->Driver->SetRenderState(D3DRS_ZENABLE, FALSE);
//		r->RenderClipper(v, pj);
//	}
//}
