#include "StdAfx.h"
#include "Shader.h"

using namespace uos;

CShader::CShader(CString const & name) : Name(name)
{
}

CShader::~CShader()
{
}

int CShader::SizeByType(CString const & t, CString const & f)
{
	if(f.empty())
	{
		if(t.StartsWith(L"float"))
		{
			auto s = t.substr(wcslen(L"float"));
	
			if(s == L"")	return sizeof(float); else
			if(s == L"2")	return 2 * sizeof(float); else
			if(s == L"3")	return 3 * sizeof(float); else
			if(s == L"4")	return 4 * sizeof(float); else
			if(s[1] == L'x') return (s[0] - L'0') * (s[2] - L'0') * sizeof(float); 
		}
	}
	else
	{
		if(f == L"Z8Y8X8W8_UNORM") return 4;
	}

	throw CException(HERE, L"Not supported");
}

bool CShader::UsesStage(int buffer, EPipelineStage stages)
{
	return Buffers[buffer].Constants.Has([stages](auto & i){ return (int(i.Stages) & int(stages)) != 0; });
}

int CShader::AddConstantBuffer(const wchar_t * name)
{
	for(auto & i : Buffers)
	{
		if(i.Name == name)
		{
			throw CException(HERE, L"ConstantBuffer already exists");
		}
	}

	Buffers.push_back({ name });
	return int(Buffers.size()) - 1;
}

void CShader::AddConstant(int buffer, CString const & name, CString const & type, EPipelineStage stages)
{
	Buffers[buffer].Constants.push_back({ name, type, SizeByType(type, L""), L"", "", -1, L"", stages });
}

void CShader::AddVertexInput(CString const & name, CString const & type, CString const & sem, CString const & f)
{
	int i = -1;
	if(sem == L"TEXCOORD")
	{
		i = VertexInput.Count([&sem](auto i){ return i.Semantics.StartsWith(sem); });
	}

	VertexInput.push_back({ name, type, SizeByType(type, f), sem, sem.ToAnsi(), i, f });
}

void CShader::AddVertexOutput(CString const & name, CString const & type, CString const & sem)
{
	int i = -1;
	if(sem == L"TEXCOORD")
	{
		i = VertexOutput.Count([&sem](auto i){ return i.Semantics.StartsWith(sem); });
	}

	VertexOutput.push_back({ name, type, 0, sem, "", i });
}

void CShader::AddTexture(CString const & name, CString const & type)
{
	Textures.push_back({ name, type });
}

void CShader::AddSampler(CString const & name)
{
	Samplers.push_back({ name });
}

void CShader::Save(CXon * r)
{
	r->Add(L"Name")->Set(Name);

	for(auto & i : Buffers)
	{
		auto c = r->Add(L"Buffer");
		c->Set(i.Name);
		for(auto & j : i.Constants)
			j.Save(c->Add(j.Name));
	}

	auto vi = r->Add(L"VertexInput");
	for(auto & i : VertexInput)
	{	
		i.Save(vi->Add(i.Name));
	}

	auto vo = r->Add(L"VertexOutput");
	for(auto & i : VertexOutput)
	{	
		i.Save(vo->Add(i.Name));
	}

	for(auto & i : Textures)
	{
		auto t = r->Add(L"Texture");
		t->Set(i.Name);
		t->Add(L"Type")->Set(i.Type);
	}

	for(auto & i : Samplers)
	{
		auto t = r->Add(L"Sampler");
		t->Set(i.Name);
	}

	r->Add(L"VertexProgram")->Set(VertexProgram.Replace(L"\n", L"   "));
	r->Add(L"PixelProgram")->Set(PixelProgram.Replace(L"\n", L"   "));
}

void CShader::Load(CXon * r)
{
	Name = r->Get<CString>(L"Name");

	CShaderDeclaration d;

	for(auto i : r->Many(L"Buffer"))
	{
		Buffers.push_back({ i->AsString() });
		for(auto j : i->Children)
		{	
			d.Load(j);
			d.Size = SizeByType(d.Type, d.Format);
			Buffers.back().Constants.push_back(d);
		}
	}

	for(auto i : r->One(L"VertexInput")->Children)
	{
		d.Load(i);
		d.Size = SizeByType(d.Type, d.Format);
		d.AnsiSemantics = d.Semantics.ToAnsi();
		VertexInput.push_back(d);
	}

	d.Size = 0;
	d.Format.clear();

	for(auto i : r->One(L"VertexOutput")->Children)
	{
		d.Load(i);
		VertexOutput.push_back(d);
	}

	for(auto i : r->Many(L"Texture"))
	{
		Textures.push_back({ i->AsString(), i->Get<CString>(L"Type") });
	}

	for(auto i : r->Many(L"Sampler"))
	{
		Samplers.push_back({ i->AsString() });
	}

	VertexProgram = r->Get<CString>(L"VertexProgram");
	PixelProgram = r->Get<CString>(L"PixelProgram");
}

CString CShader::AssembleVertexProgram()
{
	CString t;
	t.reserve(1024);

	t += L"#pragma pack_matrix(row_major)\n\n";

	for(auto i=0u; i<Buffers.size(); i++)
	{
		if(UsesStage(i, EPipelineStage::Vertex))
		{
			t += L"cbuffer " + Buffers[i].Name + L" : register(b" + CInt32::ToString(i) + L")\n{\n";

			for(auto & j : Buffers[i].Constants)
			{
				t += L"	" + j.Type + L" " + j.Name + L";\n";
			}

			t += L"};\n\n";
		}
	}

	t += L"struct Input\n{\n";

	for(auto & i : VertexInput)
	{
		t += L"	" + i.Type + L" " + i.Name + L" : " + i.Semantics + (i.SemanticsIndex != -1 ? CInt32::ToString(i.SemanticsIndex) : L"") + L";\n";
	}

	t += L"};\n\n"
	
	L"struct Output\n{\n";
	
	for(auto & i : VertexOutput)
	{
		t += L"	" + i.Type + L" " + i.Name + L" : " + i.Semantics + (i.SemanticsIndex != -1 ? CInt32::ToString(i.SemanticsIndex) : L"") + L";\n";
	}

	t+=	L"};\n\n"

		L"Output VertexMain(in Input input)\n"
		L"{\n"
		L"	Output output;\n";
	t+=	L"	" + VertexProgram;
	t+=	L"	return output;\n}\n\n";

	return t;
}

CString CShader::AssemblePixelProgram()
{
	CString t;
	t.reserve(1024);

	t += L"#pragma pack_matrix(row_major)\n\n";

	for(auto i=0u; i<Buffers.size(); i++)
	{
		if(UsesStage(i, EPipelineStage::Pixel))
		{
			t += L"cbuffer " + Buffers[i].Name + L" : register(b" + CInt32::ToString(i) + L")\n{\n";
	
			for(auto & j : Buffers[i].Constants)
			{
				t += L"	" + j.Type + L" " + j.Name + L";\n";
			}
	
			t += L"};\n\n";
		}
	}

	t += L"struct Input\n{\n";

	for(auto & i : VertexOutput)
	{
		t += L"	" + i.Type + L" " + i.Name + L" : " + i.Semantics + (i.SemanticsIndex != -1 ? CInt32::ToString(i.SemanticsIndex) : L"") + L";\n";
	}

	t+=	L"};\n\n"

	L"struct Output\n"
	L"{\n"
	L"	float4 Color   : SV_Target;\n"
	L"};\n\n";

	for(auto i=0u; i<Textures.size(); i++)
	{
		t += Textures[i].Type + L" " + Textures[i].Name + L" : register(t" + CInt32::ToString(i) + L");\n";
	}
	for(auto i=0u; i<Samplers.size(); i++)
	{
		t += L"SamplerState " + Samplers[i].Name + L" : register(s" + CInt32::ToString(i) + L");\n";
	}

	t+=	L"Output PixelMain(in Input input)\n"
		L"{\n"
		L"	Output output;\n"
		L"	" + PixelProgram;
	t+=	L"	return output;\n"
		"}\n\n";

	return t;
}

CArray<D3D11_INPUT_ELEMENT_DESC> CShader::AssebleInputLayout()
{
	CArray<D3D11_INPUT_ELEMENT_DESC> layout;

	D3D11_INPUT_ELEMENT_DESC element = { "", 0, DXGI_FORMAT_UNKNOWN, 0, 0/*offset*/, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	int offset = 0;
	
	for(auto & i : VertexInput)
	{
		element.AlignedByteOffset	= offset;
		element.SemanticName		= i.AnsiSemantics.data();
		element.SemanticIndex		= i.SemanticsIndex != -1 ? i.SemanticsIndex : 0;
		
		offset += i.Size;
		
		if(i.Format.empty())
		{
			if(i.Type.StartsWith(L"float"))
			{
				if(i.Size == 1*4)	element.Format = DXGI_FORMAT_R32_FLOAT; else
				if(i.Size == 2*4)	element.Format = DXGI_FORMAT_R32G32_FLOAT; else
				if(i.Size == 3*4)	element.Format = DXGI_FORMAT_R32G32B32_FLOAT; else
				if(i.Size == 4*4)	element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
				else
					throw CException(HERE, L"Not supported");
			}
			else
				throw CException(HERE, L"Not supported");
		}
		else
		{
			if(i.Format == L"Z8Y8X8W8_UNORM")	element.Format = DXGI_FORMAT_B8G8R8A8_UNORM; 
			else
				throw CException(HERE, L"Not supported");
		}
		
		layout.push_back(element);
	}

	return layout;
}

int CShader::GetConstantBufferSize(CString const & cbuffer)
{
	int p = 0;
	for(auto i : Buffers[FindConstantBuffer(cbuffer)].Constants)
	{
		p += i.Size;
	}

	return p;
}

int CShader::FindConstantBuffer(CString const & cbuffer)
{
	for(auto i = 0u; i<Buffers.size(); i++)
	{
		if(Buffers[i].Name == cbuffer)
		{
			return i;
		}
	}

	return -1;
}

/*
int CShader::FindConstant(int cbuffer, CString const & name)
{
	int p = 0;
	for(auto i : Constants[cbuffer].Constants)
	{
		if(i.Name == name)
			return p;

		p += i.Size;
	}

	return -1;
}
*/

int CShader::FindTexture(CString const & name)
{
	int p = 0;
	for(auto i : Textures)
	{
		if(i.Name == name)
			return p;

		p += i.Size;
	}

	return -1;
}

int CShader::FindSampler(CString const & name)
{
	int p = 0;
	for(auto i : Samplers)
	{
		if(i.Name == name)
			return p;

		p += i.Size;
	}

	return -1;
}
