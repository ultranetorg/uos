#include "stdafx.h"
#include "Pipeline.h"

using namespace uos;

CDirectPipeline::CDirectPipeline(CEngineLevel * l, CShader * s) : CEngineEntity(l)
{
	Shader = s; Shader->Take();

	GlobalConstants = new CDirectConstantBuffer();
	if(Shader->FindConstantBuffer(L"Globals") != -1)
	{
		GlobalConstants->Bind(Shader, L"Globals");
		Time = GlobalConstants->FindSlot(L"Time");
	}
		   
	Layout = s->AssebleInputLayout();
	
	///if(lg)
	///{
	///	lg->BuildShaderCode(Shader);
	///}

	Compiler = LoadLibrary(L"d3dcompiler_47.dll");
	D3DCompile = reinterpret_cast<PFN_D3DCOMPILE>(GetProcAddress(Compiler, "D3DCompile"));

	VCode = CompileShader(s->AssembleVertexProgram().ToAnsi(), "VertexMain", "vs_4_0");
	PCode = CompileShader(s->AssemblePixelProgram().ToAnsi(), "PixelMain", "ps_4_0");

	#ifdef _DEBUG
		auto id = CString::Format(L"%s-%p", GetClassName(), this);
		Level->Core->Supervisor->WriteDiagnosticFile(id + L"-VS.hlsl", s->AssembleVertexProgram());
		Level->Core->Supervisor->WriteDiagnosticFile(id + L"-PS.hlsl", s->AssemblePixelProgram());
	#endif
}

CDirectPipeline::~CDirectPipeline()
{
	delete GlobalConstants;

	for(auto & i : VShaders)
		i.second->Release();

	for(auto & i : PShaders)
		i.second->Release();

	for(auto & i : Layouts)
		i.second->Release();

	VCode->Release();
	PCode->Release();

	FreeLibrary(Compiler);

	Shader->Free();
}

void CDirectPipeline::Apply(CDirectDevice * d)
{
	if(VShaders[d] == null)
	{	
		Verify(d->DxDevice->CreateInputLayout(Layout.data(), (UINT)Layout.size(), VCode->GetBufferPointer(), VCode->GetBufferSize(), &Layouts[d]));
		Verify(d->DxDevice->CreateVertexShader(VCode->GetBufferPointer(), VCode->GetBufferSize(), nullptr, &VShaders[d]));
		Verify(d->DxDevice->CreatePixelShader(PCode->GetBufferPointer(), PCode->GetBufferSize(), nullptr, &PShaders[d]));
	}

	if(Time != -1)
	{
		GlobalConstants->SetValue(Time, Level->Core->Timer.GetTime());
		GlobalConstants->Apply(d);
	}

	d->DxContext->IASetInputLayout(Layouts[d]);
	d->DxContext->VSSetShader(VShaders[d], nullptr, 0);
	d->DxContext->PSSetShader(PShaders[d], nullptr, 0);
}

ID3DBlob * CDirectPipeline::CompileShader(CAnsiString const& code, CAnsiString const& entry_point, CAnsiString const& model)
{
	DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS;

	#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	#else
		flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
		flags |= D3DCOMPILE_AVOID_FLOW_CONTROL;
	#endif

	ID3DBlob* blob = nullptr;
	ID3DBlob* errors = nullptr;

	auto const psrc = code.c_str();
	auto const len = code.size() + 1;

	auto const hr = D3DCompile(psrc, len, nullptr, nullptr, nullptr, entry_point.c_str(), model.c_str(), flags, 0, &blob, &errors);

	if(FAILED(hr))
	{
		if(errors)
		{
			CString e;
			for(auto i : CString::FromAnsi((char *)errors->GetBufferPointer()).ToLines())
			{
				e += i + L"\n";
				Level->Log->ReportError(this, i);
			}
			errors->Release();
			Level->Core->Supervisor->WriteDiagnosticFile(L"Failed-" + Shader->Name + L"-" + CString::FromAnsi(entry_point) + L".hlsl", CString::FromAnsi(code) + L"\n\n\n" + e);
		}
		return null;
	}

	if(errors)
	{
		errors->Release();
	}

	return blob;
}

