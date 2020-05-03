#include "stdafx.h"
#include "VisualSpace.h"
#include "VisualGraph.h"

using namespace uos;

CVisualSpace::CVisualSpace(CEngineLevel * l, const CString & name) : CEngineEntity(l)
{
	Name = name;
}

CVisualSpace::~CVisualSpace()
{
}

CVisualSpace * CVisualSpace::Find(const CString & name)
{
	return Spaces.Find([name](auto i){ return i->Name == name; });
}

void CVisualSpace::AddFront(CVisualSpace * s)
{
	Spaces.push_front(s);
	s->Parent = this;
}

void CVisualSpace::AddBack(CVisualSpace * s)
{
	Spaces.push_back(s);
	s->Parent = this;
}

void CVisualSpace::AddBefore(CVisualSpace * s, CVisualSpace * before)
{
	auto i = Spaces.Findi(before);
	if(i == Spaces.end())
	{
		throw CException(HERE, L"Space not found");
	}
	Spaces.insert(i, s);
	s->Parent = this;
}

void CVisualSpace::AddAfter(CVisualSpace * s, CVisualSpace * after)
{
	auto i = Spaces.Findi(after);
	if(i == Spaces.end())
	{
		throw CException(HERE, L"Space not found");
	}
	i++;
	if(i != Spaces.end())
	{
		Spaces.insert(i, s);
	}
	else
	{
		Spaces.push_back(s);
	}
	s->Parent = this;
}

void CVisualSpace::Insert(CVisualSpace * s, CList<CVisualSpace *>::iterator pos)
{
	Spaces.insert(pos, s);
	s->Parent = this;
}
		
void CVisualSpace::Remove(CVisualSpace * s)
{
	Spaces.Remove(s);
	s->Parent = null;
}

bool CVisualSpace::IsDescedant(CVisualSpace * s)
{
	auto p = s;
	while(p && p != this)
	{
		p = p->Parent;
	}

	return p == this;
}

bool CVisualSpace::IsUnder(CVisualSpace * parent)
{
	auto p = this;
	while(p && p != parent)
	{
		p = p->Parent;
	}

	return p == parent;
}

void CVisualSpace::SetView(CView * v)
{
	View = v;
}

CView * CVisualSpace::GetView()
{
	return View;
}

CView * CVisualSpace::GetActualView()
{
	auto n = this;
	while(n && !n->View)
	{
		n = n->Parent;
	}
	return n ? n->View : null;
}

void CVisualSpace::Save(CXon * n)
{
	n->Add(L"Name")->Set(Name);
	n->Add(L"Parent")->Set(Parent->Name);
}

void CVisualSpace::Load(CXon * n)
{
	Name = n->Get<CString>(L"Name");
	ParentName = n->Get<CString>(L"Parent");
}

void CVisualSpace::AddGraph(CVisualGraph * g)
{
	Graphs.push_back(g); // graphs to space
	g->AddSpace(this);
}

void CVisualSpace::RemoveGraph(CVisualGraph * g)
{
	Graphs.Remove(g); // graphs to space
	g->RemoveSpace(this);
}