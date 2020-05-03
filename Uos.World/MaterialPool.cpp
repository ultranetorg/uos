#include "stdafx.h"
#include "MaterialPool.h"

using namespace uos;

CMaterialPool::CMaterialPool(CEngine * g)
{
	Engine = g;
}

CMaterialPool::~CMaterialPool()
{
	for(auto i : Materials)
	{
		i->Free();
	}
}

CMaterial * CMaterialPool::GetMaterial(const CString & name)
{
	if(name.empty())
	{
		return null;
	}
			
	auto m = Materials.Find([name](auto i){ return i->Name == name; });
	if(m == null)
	{
		auto parts = name.Split(L" ");
		if(parts.size() == 3)
		{
			m = new CMaterial(&Engine->EngineLevel, Engine->PipelineFactory->DiffuseColorShader);

			float r = CFloat::Parse(parts[0]);
			float g = CFloat::Parse(parts[1]);
			float b = CFloat::Parse(parts[2]);

			m->Float4s[L"DiffuseColor"] = CFloat4(r, g, b, 1.f); ///SetDiffuseColor(r, g, b);
		}
		else if(parts.size() == 4)
		{
			m = new CMaterial(&Engine->EngineLevel, Engine->PipelineFactory->DiffuseColorShader);

			float r = CFloat::Parse(parts[0]);
			float g = CFloat::Parse(parts[1]);
			float b = CFloat::Parse(parts[2]);
			float a = CFloat::Parse(parts[3]);

			m->AlphaBlending = a < 1;
			m->Float4s[L"DiffuseColor"] = CFloat4(r, g, b, a); ///SetDiffuseColor(r, g, b);
		}
	
		if(m)
		{
			m->Name = name;
			Materials.push_back(m);
		}
	}
	return m;
}

CMaterial * CMaterialPool::GetMaterial(CFloat4 & c)
{
	return GetMaterial(c.ToString());
}

CMaterial * CMaterialPool::GetMaterial(CFloat3 & c)
{
	return GetMaterial(c.ToString());
}

