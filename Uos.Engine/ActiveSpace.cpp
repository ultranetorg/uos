#include "stdafx.h"
#include "ActiveSpace.h"
#include "ActiveGraph.h"


using namespace uos;

CActiveSpace::CActiveSpace(const CString & n)
{
	Name = n;
}

CActiveSpace::~CActiveSpace()
{
	for(auto i : Spaces)
		i->Free();
}

CActiveSpace * CActiveSpace::Find(const CString & name)
{
	return Spaces.Find([name](auto i){ return i->Name == name; });
}

void CActiveSpace::AddFront(CActiveSpace * s)
{
	s->Take();
	Spaces.push_front(s);
	s->Parent = this;
}

void CActiveSpace::AddBack(CActiveSpace * s)
{
	s->Take();
	Spaces.push_back(s);
	s->Parent = this;
}

void CActiveSpace::AddBefore(CActiveSpace * s, CActiveSpace * p)
{
	s->Take();
	auto i = Spaces.Findi(p);

	if(i == Spaces.end())
	{
		throw CException(HERE, L"Space not found");
	}

	Spaces.insert(i, s);
	s->Parent = this;
}

void CActiveSpace::AddAfter(CActiveSpace * s, CActiveSpace * p)
{
	s->Take();
	auto i = Spaces.Findi(p);

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

void CActiveSpace::Insert(CActiveSpace * s, CList<CActiveSpace *>::iterator pos)
{
	s->Take();
	Spaces.insert(pos, s);
	s->Parent = this;
}

void CActiveSpace::Remove(CActiveSpace * s)
{
	s->Parent = null;
	Spaces.Remove(s);
	s->Free();
}

bool CActiveSpace::IsDescedant(CActiveSpace * s)
{
	auto p = s;
	while(p && p != this)
	{
		p = p->Parent;
	}

	return p == this;
}

bool CActiveSpace::IsUnder(CActiveSpace * parent)
{
	auto p = this;
	while(p && p != parent)
	{
		p = p->Parent;
	}

	return p == parent;
}

void CActiveSpace::SetView(CView * v)
{
	View = v;
}

CView * CActiveSpace::GetView()
{
	return View;
}

CView * CActiveSpace::GetActualView()
{
	auto n = this;
	while(n && !n->View)
	{
		n = n->Parent;
	}
	return n ? n->View : null;
}

void CActiveSpace::Save(CXon * n)
{
	n->Add(L"Name")->Set(Name);
	n->Add(L"Parent")->Set(Parent->Name);
}

void CActiveSpace::Load(CXon * n)
{
	Name = n->Get<CString>(L"Name");
	ParentName = n->Get<CString>(L"Parent");
}

void CActiveSpace::AddGraph(CActiveGraph * g)
{
	Graphs.push_back(g); // graphs to space
	g->Spaces[this];
}

void CActiveSpace::RemoveGraph(CActiveGraph * g)
{
	Graphs.Remove(g); // graphs to space
	g->RemoveSpace(this);
}