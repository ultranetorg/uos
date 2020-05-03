#include "stdafx.h"
//#include "Area.h"
//#include "Unit.h"
//#include "PositioningArea.h"
#include "WorldServer.h"

using namespace uos;

CArea::CArea(CWorldServer * l, CString const & name)
{
	Level = l;
	Name = name;
}

CArea::~CArea()
{
	if(Parent)
		Level->Log->ReportError(this, L"Destruction of attached area");

	while(auto i = Areas.First())
	{
		Forget(i->Name);
	}

	for(auto & i : VisualSpaces)
		delete i.Space;

	for(auto & i : ActiveSpaces)
		i.Space->Free();
}

void CArea::SetView(CView * v)
{
	View = v;
}

CSpaceBinding<CVisualSpace> & CArea::AllocateVisualSpace(CViewport * vp)
{
	auto space = Level->Engine->CreateVisualSpace(Name);
	space->SetView(GetActualView());
	
	CSpaceBinding<CVisualSpace> b;
	b.Space = space;
	b.Viewport = vp;
	b.Tags = vp->Tags;
	VisualSpaces.push_back(b);

	return VisualSpaces.back();
}

CSpaceBinding<CActiveSpace> & CArea::AllocateActiveSpace(CViewport * vp)
{
	auto space = Level->Engine->CreateActiveSpace(Name);
	space->SetView(GetActualView());
	
	CSpaceBinding<CActiveSpace> b;
	b.Space = space;
	b.Viewport = vp;
	b.Tags = vp->Tags;
	ActiveSpaces.push_back(b);

	return ActiveSpaces.back();
}

void CArea::DeallocateVisualSpace(CVisualSpace * s)
{
	VisualSpaces.Remove(s);
	delete s;
}

void CArea::DeallocateActiveSpace(CActiveSpace * s)
{
	ActiveSpaces.Remove(s);
	s->Free();
}

bool CArea::Add(CArea * a, EAddLocation l)
{
	auto p = FindPlacement(a);

	bool newp = p == null;

	if(!p)
	{
		if(a->Parent)
			throw CException(HERE, L"Remove before adding");

		p = new CPlacement();
		p->Name = a->Name;
		p->Class = a->GetInstanceName();
		p->Lifespan = a->Lifespan;

		if(l == EAddLocation::Front || l == EAddLocation::Known)	Areas.AddFront(p); else
		if(l == EAddLocation::Back)									Areas.AddBack(p);
	}
	else
	{
		if(l == EAddLocation::Known)
		{
		}
		else if(l == EAddLocation::Front)
		{
			Areas.Remove(p);
			Areas.AddFront(p);
		}
		else if(l == EAddLocation::Back)
		{
			Areas.Remove(p);
			Areas.AddBack(p);
		}
	}

	a->Parent = this;
	a->TransformationParent = this;
	p->Area = a;
	a->Take();

	for(auto & i : VisualSpaces)
	{
		if(a->Tags.Contains(L"all") || i.Tags.Contains(L"any") || i.Tags.Has([a](auto & i){ return a->Tags.Contains(i); }))
		{
			auto s = i.Space->Spaces.begin();
			auto j = Areas.begin();
	
			while((*j)->Area != a && s != i.Space->Spaces.end())
			{
				if((*j)->Area)
					s++;
				j++;
			}

			auto vs = a->AllocateVisualSpace(i.Viewport).Space;
			vs->Matrix = CMatrix(Transformation);

			i.Space->Insert(vs, s);
		}
	}

	for(auto & i : ActiveSpaces)
	{
		if(a->Tags.Contains(L"all") || i.Tags.Contains(L"any") || i.Tags.Has([a](auto & i){ return a->Tags.Contains(i); }))
		{
			auto s = i.Space->Spaces.begin();
			auto j = Areas.begin();
	
			while((*j)->Area != a && s != i.Space->Spaces.end())
			{
				if((*j)->Area)
					s++;
				j++;
			}
	
			auto vs = a->AllocateActiveSpace(i.Viewport).Space;
			vs->Matrix = CMatrix(Transformation);

			i.Space->Insert(vs, s);
		}
	}

	a->Transform(a->Transformation);

	return newp;
}

void CArea::Remove(CArea * a)
{
	auto p = Areas.Find([a](auto i){ return i->Area == a; });

	if(!p)
		throw CException(HERE, L"Placement not found");

	auto vs = a->VisualSpaces;
	for(auto & i : vs)
	{	
		i.Space->Parent->Remove(i.Space);
		a->DeallocateVisualSpace(i.Space);
	//	delete i.Space;
	}
	
	auto as = a->ActiveSpaces;
	for(auto & i : as)
	{	
		i.Space->Parent->Remove(i.Space);
		a->DeallocateActiveSpace(i.Space);
	//	i.Space->Free();
	}

	//a->VisualSpaces.clear();
	//a->ActiveSpaces.clear();

	p->Area = null;
	a->Parent = null;
	a->Free();
}

void CArea::Open(CArea * a, EAddLocation l, CViewport * vp, CPick & pick, CTransformation & origin)
{
	auto newp = Add(a, l);

	if(auto u = a->As<CUnit>())
	{
		auto p = FindPlacement(a);

		auto s = u->DetermineSize(u->GetActualMaxSize(vp), p->Size);
		auto t = u->DetermineTransformation(null, pick, p->Transformation);

		u->Update();

		a->Transform(t);

		u->Open();
	}
}

void CArea::Close(CArea * a)
{
	if(auto u = a->As<CUnit>())
	{
		u->Close();
	}
	Remove(a);
}

void CArea::Remember(CArea * a)
{
	auto p = FindPlacement(a);

	if(p)
	{
		p->Transformation = a->Transformation;
		p->Size = a->Measure();
	}
}

void CArea::Forget(CString const & name)
{
	auto p = Areas.Find([name](auto i){ return i->Name == name; });

	if(p->Area)
	{
		Close(p->Area);
	}

	Areas.Remove(p);
	delete p;
}

void CArea::Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin)
{
}

void CArea::Save()
{
	CTonDocument d;
	Save(&d);
	auto f = Level->Storage->OpenWriteStream(Level->Storage->MapPath(UOS_MOUNT_USER_GLOBAL, CPath::Join(Directory, GetClassName() + L".area")));
	d.Save(&CXonTextWriter(f));
	Level->Storage->Close(f);
}

void CArea::Save(CXon * x)
{
	x->Add(L"Interactive")->Set(Interactive);
	x->Add(L"Tags")->Set(CString::Join(Tags, L" "));

	for(auto i : Areas)
	{
		if(i->Lifespan == ELifespan::Permanent)
		{
			auto a = x->Add(L"Area");
			a->Id = i->Name;
			a->Add(L"Size")->Set(i->Size);
			a->Add(L"Transformation")->Set(i->Transformation);
					
			if(i->Area)
			{
				i->Area->Save(a);
			}
		}
	}
}

void CArea::Load()
{
	auto s = Level->Storage->OpenReadStream(Level->Storage->MapPath(UOS_MOUNT_USER_GLOBAL, CPath::Join(Directory, GetClassName() + L".area")));
	Load(&CTonDocument(CXonTextReader(s)));
	Level->Storage->Close(s);
}

void CArea::Load(CXon * p)
{
	Interactive	= p->Get<CBool>(L"Interactive");
	Tags		= p->Get<CString>(L"Tags").SplitToList(L" ");
	Lifespan	= ELifespan::Permanent;
	MaxSize		= [](CViewport * c){ return CSize(c->W, c->H, FLT_MAX); };
				    
	for(auto i : p->Many(L"Area"))
	{
		auto p = new CPlacement();
		p->Name				= i->Id;
		p->Lifespan			= ELifespan::Permanent;
		p->Size				= i->Get<CSize>(L"Size");
		p->Transformation	= i->Get<CTransformation>(L"Transformation");
		///a->Layout		= i->Get<CString>(L"Layout");
		
		Areas.AddBack(p);	

		CArea * a = null;
				
		if(CUol::GetObjectType(i->Id) == CArea::GetClassName())				a = new CArea(Level, i->Id); else
		if(CUol::GetObjectType(i->Id) == CPositioningArea::GetClassName())	a = new CPositioningArea(Level, i->Id);

		if(a)
		{
			a->Load(i);
	
			Add(a, EAddLocation::Back);
	
			a->Free();
		}
	}
}

bool CArea::ContainsDescedant(CArea * a)
{
	auto p = a;

	while(p && p != this)
	{
		p = p->Parent;
	}

	return p == this;
}

bool CArea::IsUnder(CArea * parent)
{
	auto p = this;

	while(p && p != parent)
	{
		p = p->Parent;
	}

	return p == parent;
}

bool CArea::UnderInteractive()
{
	auto p = Parent;
	while(p && !p->Interactive)
	{
		p = p->Parent;
	}
	return p && p->Interactive;
}

CArea * CArea::Find(CString const & name)
{
	std::function<CArea * (CArea *)> f;

	f = [name, &f](CArea * s) -> CArea *
	{
		if(s->Name == name)
			return s;

		for(auto i : s->Areas)
			if(i->Area)
				if(auto a = f(i->Area))
					return a;

		return null;
	};

	return f(this);
}

CArea * CArea::Match(CString const & tag)
{
	std::function<CArea * (CArea *)> f;

	f = [tag, &f](CArea * s) -> CArea *
		{
			if(s->Tags.Contains(tag))
				return s;

			for(auto i : s->Areas)
				if(i->Area)
					if(auto a = f(i->Area))
						return a;

			return null;
		};

	return f(this);
}

CSize CArea::GetActualMaxSize(CViewport * vp)
{
	auto p = this;

	while(p && !p->MaxSize)
	{
		p = p->Parent;
	}

	return p ? p->MaxSize(vp) : CSize::Nan;
}

CView * CArea::GetActualView()
{
	auto p = this;

	while(p && !p->View)
	{
		p = p->Parent;
	}

	return p ? p->View : null;
}

void CArea::Transform(const CTransformation & t)
{
	Transformation = t;

	auto & m = CMatrix(t);

	if(TransformationParent)
	{
		m = m * CMatrix(TransformationParent->Transformation);
	}

	for(auto & i : VisualSpaces)
		i.Space->Matrix = m;

	for(auto & i : ActiveSpaces)
		i.Space->Matrix = m;


	for(auto i : Areas)
	{
		if(i->Area)
			i->Area->Transform(i->Area->Transformation);
	}
}

CPlacement * CArea::FindPlacement(CArea * a)
{
	for(auto i : Areas)
	{
		if(i->Name == a->Name)
		{
			return i;
		}
	}

	return null;
}

CSize CArea::Measure()
{
	return CSize::Empty;
}