#include "StdAfx.h"
#include "PipelineFactory.h"

using namespace uos;

CDirectPipelineFactory::CDirectPipelineFactory(CEngineLevel * l, CDirectSystem * ge) : CEngineEntity(l)
{
	GraphicEngine = ge;
	
	//for(auto  i : ge->Devices)
	//{
	//	DWORD vsv = i->Display->Capabilities.VertexShaderVersion;
	//	DWORD psv = i->Display->Capabilities.PixelShaderVersion;
	//
	//	Level->Log->ReportMessage(this, L"Vertex Shaders %d.%d    Pixel Shaders %d.%d", D3DSHADER_VERSION_MAJOR(vsv), D3DSHADER_VERSION_MINOR(vsv), D3DSHADER_VERSION_MAJOR(psv), D3DSHADER_VERSION_MINOR(psv));
	//
	//	if(psv < D3DPS_VERSION(2, 0))
	//	{
	//		throw CAttentionException(HERE, L"Sorry, your video adapter is too weak. It does not support Pixel Shaders 2.0");
	//	}
	//}

	//->AddConstant(geometry, L"CameraPosition",	L"float3", 3);
	//->AddConstant(geometry, L"W",				L"matrix", 16);
	//->AddConstant(geometry, L"WVP",				L"matrix", 16);

	DiagGrid.AddColumn(L"Name");
	Diagnostic = Level->Core->Supervisor->CreateDiagnostics(L"Pipelines");
	Diagnostic->Updating += ThisHandler(OnDiagnosticUpdating);
	
	{
		DiffuseColorShader = new CShader(L"SolidColor");
		auto geometry = DiffuseColorShader->AddConstantBuffer(L"Geometry");
		auto material = DiffuseColorShader->AddConstantBuffer(L"Material");
		DiffuseColorShader->AddConstant(geometry, L"WVP", L"float4x4", EPipelineStage::Vertex);
		DiffuseColorShader->AddVertexInput(L"Position", L"float3", L"POSITION");
		DiffuseColorShader->AddVertexOutput(L"PositionWVP", L"float4", L"SV_POSITION");
		DiffuseColorShader->VertexProgram = L"output.PositionWVP = mul(float4(input.Position, 1.0f), WVP);";
		DiffuseColorShader->AddConstant(material, L"DiffuseColor", L"float4", EPipelineStage::Pixel);
		DiffuseColorShader->PixelProgram = L"output.Color = DiffuseColor;";
	}

	{
		DiffuseTextureShader = new CShader(L"SimpleTexture");
		auto geometry = DiffuseTextureShader->AddConstantBuffer(L"Geometry");
		DiffuseTextureShader->AddConstant(geometry, L"WVP",	L"float4x4", EPipelineStage::Vertex);
		DiffuseTextureShader->AddVertexInput(L"Position", L"float3", L"POSITION");
		DiffuseTextureShader->AddVertexInput(L"UV", L"float2", L"TEXCOORD");
		DiffuseTextureShader->AddVertexOutput(L"PositionWVP", L"float4", L"SV_POSITION");
		DiffuseTextureShader->AddVertexOutput(L"UV", L"float2", L"TEXCOORD");
		DiffuseTextureShader->VertexProgram =L"output.PositionWVP = mul(float4(input.Position, 1.0f), WVP);"
											 L"output.UV = input.UV;";
		DiffuseTextureShader->AddTexture(L"DiffuseTexture", L"Texture2D");
		DiffuseTextureShader->AddSampler(L"DiffuseSampler");
		DiffuseTextureShader->PixelProgram = L"output.Color = DiffuseTexture.Sample(DiffuseSampler, input.UV);";
	}
	
	{
		TextShader = new CShader(L"Text");
		auto geometry = TextShader->AddConstantBuffer(L"Geometry");
		auto material = TextShader->AddConstantBuffer(L"Material");
		TextShader->AddConstant(geometry, L"WVP",	L"float4x4", EPipelineStage::Vertex);
		TextShader->AddVertexInput(L"Position", L"float3", L"POSITION");
		TextShader->AddVertexInput(L"UV",		L"float2", L"TEXCOORD");
		TextShader->AddVertexOutput(L"PositionWVP", L"float4", L"SV_POSITION");
		TextShader->AddVertexOutput(L"UV", L"float2", L"TEXCOORD");
		TextShader->VertexProgram =	L"output.PositionWVP = mul(float4(input.Position, 1.0f), WVP);"
									L"output.UV	= input.UV;";
		TextShader->AddConstant(material, L"Color", L"float4", EPipelineStage::Pixel);
		TextShader->AddTexture(L"DiffuseTexture", L"Texture2D");
		TextShader->AddSampler(L"DiffuseSampler");
		TextShader->PixelProgram =	L"float4 t = DiffuseTexture.Sample(DiffuseSampler, input.UV);"
									L"clip((t.x + t.y + t.z)/3 - 0.5);"
									L"output.Color.xyz = Color;";
	}
}

CDirectPipelineFactory::~CDirectPipelineFactory()
{
	for(auto i : Pipelines)
	{
		i->Free();
	}

	DiffuseColorShader->Free();
	DiffuseTextureShader->Free();
	TextShader->Free();
}

void CDirectPipelineFactory::OnDiagnosticUpdating(CDiagnosticUpdate & u)
{
	for(auto i : Pipelines)
	{
		Diagnostic->Add(i->Shader->Name);
		Diagnostic->Add(L"  VertexInput:");
		for(auto & j : i->Shader->VertexInput)
		{
			Diagnostic->Add(L"    %s %s %d %s", j.Name, j.Type, j.Size, j.Semantics);
		}

		Diagnostic->Add(L"  Constants:");
		Diagnostic->Add(L"    " + i->Shader->Name);
		for(auto & j : i->Shader->Buffers)
		{
			Diagnostic->Add(L"      " + j.Name);
			for(auto & k : j.Constants)
			{
				Diagnostic->Add(L"        %s %s %d", k.Name, k.Type, k.Size);
			}
		}
		Diagnostic->Add(L"  Samplers: ");
		for(auto & j : i->Shader->Samplers)
		{
			Diagnostic->Append(L"    " + j.Name);
		}
		Diagnostic->Add(L"  Textures: ");
		for(auto & j : i->Shader->Textures)
		{
			Diagnostic->Add(L"    " + j.Name + L" : " + j.Type);
		}
	}

	//Diagnostic->Add(u, DiagGrid);
}

CDirectPipeline * CDirectPipelineFactory::GetPipeline(CShader * s)
{
	for(auto i : Pipelines)
	{
		if(i->Shader == s || *i->Shader == *s)
		{
			i->Take();
			return i;
		}
	}

	auto p = new CDirectPipeline(Level, s);

	Pipelines.push_back(p);
	
	p->Take();
	return p;
}
