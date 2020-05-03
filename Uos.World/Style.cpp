#include "stdafx.h"
#include "Style.h"

using namespace uos;

CStyle::CStyle(CEngine * e, CMaterialPool * p)
{
	Engine = e;
	Pool = p;
	Document = new CTonDocument();
}

CStyle::CStyle(CEngine * e, CMaterialPool * p, IXonReader & r)
{
	Engine = e;
	Pool = p;
	Document = new CTonDocument(r);

	CMap<CString, CString> consts;

	std::function<void(CXon *)> compile;

	compile = [&compile, &consts](CXon * p)
	{
		if(p->Name[0] == L'@')
		{
			consts[p->Name] = p->AsString();
		}

		if(p->Value && p->AsString()[0] == L'@')
		{
			p->Set(consts[p->AsString()]);
		}

		for(auto i : p->Children)
		{
			compile(i);
		}
	};

	compile(Document);
}

CStyle::~CStyle()
{
	delete Document;

	for(auto & i : Meshes)
	{
		i.second->Free();
	}
}

CMaterial * CStyle::GetMaterial(CString const & name)
{
	auto p = Document->One(name);
	return Pool->GetMaterial(p->AsString());
}

CFont * CStyle::GetFont(CString const & name)
{
	auto p = Document->One(name);
	return Engine->FontFactory->GetFont(p->Get<CFontDefinition>());
}

CAnimation CStyle::GetAnimation(CString const & name)
{
	auto p = Document->One(name);
	return CAnimation(p);
}

CMesh * CStyle::GetMesh(CString const & name)
{
	return Meshes(name);
}

void CStyle::DefineMesh(CString const & name, CMesh * mesh)
{
	Meshes[name] = mesh;
	mesh->Take();
}

CStyle * uos::CStyle::Clone()
{
	CStyle * style = new CStyle(Engine, Pool);

	std::function<CXon *(CXon *, CXon *)> f;

	f = [&f, this](CXon * a, CXon * parent)
		{
			auto p = new CXon(a->Name);
			p->Parent = parent;

			for(auto i : a->Children)
			{
				p->Children.push_back(f(i, p));
			}

			for(auto i : a->Templates)
			{
				p->Templates.push_back(f(i, p));
			}

			if(a->Value)
			{
				p->Value = a->Value->Clone();
			}

			return p;
		};

	for(auto i : Document->Children)
	{
		style->Document->Children.push_back(f(i, style->Document));
	}

	for(auto & i : Meshes)
	{
		style->DefineMesh(i.first, i.second);
	}

	return style;
}
