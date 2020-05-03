#include "StdAfx.h"
#include "Material.h"

using namespace uos;

CMaterial::CMaterial(CEngineLevel * l, CShader * sh, CString const & name) : CEngineEntity(l)
{
	Name = name;
	Bind(sh);
}

CMaterial::CMaterial(CEngineLevel * l, CString const & name) : CEngineEntity(l)
{
	Name = name;
}

CMaterial::~CMaterial()
{
	delete Buffer;

	Shader->Free();
}

void CMaterial::Bind(CShader * sh)
{
	Shader = sh_assign(Shader, sh);

	auto slot = sh->FindConstantBuffer(L"Material");

	if(slot != -1)
	{
		Buffer = new CDirectConstantBuffer();
		Buffer->Bind(sh, L"Material");

		for(auto & i : sh->Buffers[slot].Constants)
		{
			if(i.Type == L"float")
			{
				Floats[i.Name].Slot = Buffer->FindSlot(i.Name);
			}
			else if(i.Type == L"float3")
			{
				Float3s[i.Name].Slot = Buffer->FindSlot(i.Name);
			}
			else if(i.Type == L"float4")
			{
				Float4s[i.Name].Slot = Buffer->FindSlot(i.Name);
			}
			else if(i.Type == L"float4x4" || i.Type == L"matrix")
			{
				Matrixes[i.Name].Slot = Buffer->FindSlot(i.Name);
			}
			else
				throw CException(HERE, L"Not supported");
		}
	}

	int t = 0;
	for(auto i : sh->Textures)
	{
		Textures[i.Name].Slot = t++;
	}

	int s = 0;
	for(auto i : sh->Samplers)
	{
		Samplers[i.Name].Slot = s++;
	}
}

void CMaterial::Apply(CDirectDevice * d)
{
	if(Buffer)
	{
		for(auto & i : Floats)
			if(i.second.Changed)
			{
				Buffer->SetValue(i.second.Slot, i.second.Value);
				i.second.Changed = false;
			}

		for(auto & i : Float3s)
			if(i.second.Changed)
			{
				Buffer->SetValue(i.second.Slot, i.second.Value);
				i.second.Changed = false;
			}

		for(auto & i : Float4s)
			if(i.second.Changed)
			{
				Buffer->SetValue(i.second.Slot, i.second.Value);
				i.second.Changed = false;
			}

		for(auto & i : Matrixes)
			if(i.second.Changed)
			{
				Buffer->SetValue(i.second.Slot, i.second.Value);
				i.second.Changed = false;
			}

		Buffer->Apply(d);
	}

	for(auto & i : Textures)
		i.second.Texture->Apply(d, i.second.Slot);

	for(auto & i : Samplers)
		i.second.Apply(d);
}


void CMaterial::Save(CXon * r, IMaterialStore * st)
{
	r->Set(Name);
	r->Add(L"Shader")->Set(Shader->Name);
	
	if(AlphaBlending)
		r->Add(L"AlphaBlending")->Set(AlphaBlending);

	for(auto & i : Floats)
		r->Add(i.first)->Set(i.second.Value);
	for(auto & i : Float3s)
		r->Add(i.first)->Set(i.second.Value);
	for(auto & i : Float4s)
		r->Add(i.first)->Set(i.second.Value);
	for(auto & i : Matrixes)
		r->Add(i.first)->Set(i.second.Value);

	for(auto & i : Textures)
	{
		auto p = r->Add(L"Texture");
		p->Set(i.first);
		p->Add(L"Data")->Set(i.second.Texture->Name);
	}
	for(auto & i : Samplers)
	{
		auto p = r->Add(L"Sampler");
		p->Set(i.first);
		i.second.Save(p);
	}
}

void CMaterial::Load(CXon * r, IMaterialStore * st)
{
	Name = r->Get<CString>();
//	Shader = st->GetShader(r->Get<CString>(L"Shader"));

	if(r->One(L"AlphaBlending"))
		AlphaBlending = r->One(L"AlphaBlending")->AsBool();

	for(auto i : r->Children)
	{
		if(dynamic_cast<CFloat *>(i->Value))
		{
			Floats[i->Name] = i->AsFloat32();
		}
		else if(dynamic_cast<CFloat3 *>(i->Value))
		{
			Float3s[i->Name] = i->Get<CFloat3>();
		}
		else if(dynamic_cast<CFloat4 *>(i->Value))
		{
			Float4s[i->Name] = i->Get<CFloat4>();
		}
	}
			
	for(auto i : r->Many(L"Texture"))
	{
		auto t = st->GetTexture(i->Get<CString>(L"Data"));
		Textures[i->AsString()] = t;
	}

	for(auto i : r->Many(L"Sampler"))
	{
		Samplers[i->AsString()].Load(i);
	}
}
