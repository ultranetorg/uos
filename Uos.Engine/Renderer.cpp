#include "StdAfx.h"
#include "Renderer.h"
#include "VisualGraph.h"

using namespace uos;

CRenderer::CRenderer(CEngineLevel * l, CDirectSystem * ge, CDirectPipelineFactory * sf, CMaterialFactory * mf) : CEngineEntity(l)
{
	GraphicEngine	= ge;
	PipelineFactory	= sf;
		
	Diagnostic = Level->Core->Supervisor->CreateDiagnostics(L"Renderer");
	Diagnostic->Updating += ThisHandler(OnDiagnosticsUpdate);

	PcUpdate = new CPerformanceCounter(L"Render update");
	Level->Core->AddPerformanceCounter(PcUpdate);
}
	
CRenderer::~CRenderer()
{
	delete PcUpdate;;
	Diagnostic->Updating -= ThisHandler(OnDiagnosticsUpdate);
}

void CRenderer::OnDiagnosticsUpdate(CDiagnosticUpdate & u)
{
	CDiagGrid g;
	g.AddColumn(L"Class");
	g.AddColumn(L"Name");
	g.AddColumn(L"Time (s)");
	g.AddColumn(L"Tags");
	g.AddColumn(L"View");
	g.AddColumn(L"Info");

	std::function<void(CScreenRenderLayer *, CVisualSpace *, const CString &)> dump = [&](CScreenRenderLayer * t, CVisualSpace * s, const CString & tab)
																{
																	if(Diagnostic->ShouldProceed(u, g.GetSize()))
																	{
																		auto & r = g.AddRow();
	
																		if(Diagnostic->ShouldFill(u, g.GetSize()))
																		{
																			auto dt = s->Matrix.Decompose();

																			r.SetNext(L"Space");
																			r.SetNext(tab + s->Name);
																			r.SetNext(L"%6.0f - %3lld", 1e6 * s->PcRendering[t->Viewport].GetTime() / s->PcRendering[t->Viewport].GetMeasures(), s->PcRendering[t->Viewport].GetMeasures());
																			r.SetNext(CString::Join(t->Viewport->Tags, L" "));
																			r.SetNext(s->GetActualView() ? s->GetActualView()->GetName() : L"");
																			r.SetNext(dt.Position.ToNiceString() + L" " + dt.Rotation.ToNiceString() + L" " + dt.Scale.ToNiceString());

																			s->PcRendering[t->Viewport].Reset();

																			for(auto j : s->Graphs)
																			{
																				auto & r = g.AddRow();
																				r.SetNext(L"Graph");
																				r.SetNext(tab + L"  " + j->Name);
																			}
																		}
	
																		for(auto i : s->Spaces)
																		{
																			dump(t, i, tab + L"  ");
																		}
																	}
																};

	for(auto i : Layers)
	{
		auto & r = g.AddRow();
		r.SetNext(L"Target");
		r.SetNext(L"%p", i);
		r.SetNext(L"");
		r.SetNext(L"%s", CString::Join(i->Viewport->Target->Screen->Tags, L" "));
		r.SetNext(L"");
		r.SetNext(L"%s", i->Viewport->Target->Screen->Name);

		dump(i, i->Space, L"  ");
	}

	Diagnostic->Add(u, g);


/*
	for(auto i : DiagItems)
	{
		Diagnostic->Add(*i);
	}*/

}

CScreenRenderLayer * CRenderer::AddLayer(CDisplayDevice * dd)
{
	//CDirectDevice * d = new CDirectDevice(Level, this, dd);
	//d->Index = Devices.size();
	//Devices.push_back(d);
	//return d->AddTarget();
	throw CException(HERE, L"Not implemented");
}

CScreenRenderLayer * CRenderer::AddLayer(CScreenViewport * svp, CVisualSpace * space)
{
	auto l = new CScreenRenderLayer(Level);
	l->Viewport = svp;
	l->Space = space;

	if(!ScreenTargets.Contains(svp->Target))
	{
		///st = GraphicEngine->GetAppropriateDevice(ws)->AddTarget(ws);
		ScreenTargets.push_back(svp->Target);
	}

	Layers.push_back(l);
	return l;
}

void CRenderer::RemoveTarget(CScreenRenderLayer * l)
{
	auto t = l->Viewport->Target;

	Layers.Remove(l);
	delete l;

	if(Layers.Count([t](auto i){ return t == i->Viewport->Target; }) == 0)
	{
		///st->Device->RemoveTarget(st);
		ScreenTargets.Remove(t);
	}
}
	
void CRenderer::Update()
{
	PcUpdate->BeginMeasure();

	for(auto t : ScreenTargets)
	{
		t->Apply();
		
		float c[4] = {0, 0, 0, 1};
		t->Device->DxContext->ClearRenderTargetView(t->RenderTargetView, c);

		for(auto l : Layers)
		{
			if(l->Viewport->Target == t)
			{
				l->Viewport->Apply();
				RenderSpace(t, l->Viewport, l->Space);
			}
		}
		
		t->Present();
	}

	PcUpdate->EndMeasure();
}

void CRenderer::RenderSpace(CRenderTarget * t, CViewport * vp, CVisualSpace * s)
{	
	auto st = t->As<CScreenRenderTarget>();

	s->PcRendering[vp].BeginMeasure();
	
	st->Device->DxContext->ClearDepthStencilView(st->DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);

	for(auto g : s->Graphs)
	{
		g->Render(t, vp, s);
	}

	s->PcRendering[vp].EndMeasure();
		
	for(auto i = s->Spaces.rbegin(); i != s->Spaces.rend(); i++)
	{
		RenderSpace(t, vp, *i);
	}
}
