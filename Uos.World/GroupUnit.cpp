#include "stdafx.h"
#include "GroupUnit.h"
#include "WorldServer.h"

using namespace uos;

CGroupUnit::CGroupUnit(CWorldServer * l, CString & dir, CUol & entity, CString const & type, CView * hview, CVisualGraph * vg, CActiveGraph * ag) : CUnit(l, hview, vg, ag, entity.Object)
{
	Directory = dir;
	View = hview;
	Tags = {L"Apps"};
	Lifespan = ELifespan::Permanent;

	Header = new CHeader(Level, vg, ag, HeaderView);
	Header->Tags = {L"all"};

	Entity = l->FindObject(entity)->As<CGroup>();

	for(auto i : Entity->As<CGroup>()->Entities)
	{
		AddModel(Level->GenerateAvatar(i.Url, Level->Complexity), null);
	}
}

CGroupUnit::CGroupUnit(CWorldServer * l, CString & dir, CString const & name, CView * hview, CVisualGraph * vg, CActiveGraph * ag) : CUnit(l, hview, vg, ag, name)
{
	Header = new CHeader(Level, vg, ag, HeaderView);
	Header->Tags = {L"all"};

	Directory = dir;
	View = hview;
	Tags = {L"Apps"};
	Lifespan = ELifespan::Permanent;
}

CGroupUnit::~CGroupUnit()
{
	for(auto i : Header->Tabs)
	{
		if(i->Model)
		{
			if(i->Model->Active->Parent == ActiveGraph->Root)
				ActiveGraph->Root->RemoveNode(i->Model->Active);
	
			i->Model->Save();
		
			Level->DestroyAvatar(i->Model);
		}
	}

	Header->Free();
	Header = null;
}

CHeaderTab * CGroupUnit::AddModel(CUol & m, CModel * model)
{
	return Header->AddTab(m, model);
}

void CGroupUnit::OpenModel(CUol & m)
{
	if(auto t = Header->Tabs.Find([](auto i){ return i->Shown; }))
	{
		CloseModel(t->Model.Url);
	}
	
	auto t = Header->Tabs.Find([&m](auto i){ return i->Model.Url == m; });

	if(!t->Model)
	{
		auto dir = CPath::Join(Directory, CNativePath::GetSafe(CUol(t->Model.Url.Parameters(L"entity")).Object));

		t->Model = Level->CreateAvatar(m, dir)->As<CModel>();
		t->Model->Unit = this;
		
		//t->Model->Destroying += [](auto i){};

		if(!t->Model->Active->Parent)
			ActiveGraph->Root->AddNode(t->Model->Active);

		if(Size.W > 0 && Size.H > 0)
		{
			t->Model->DetermineSize(Size, t->Size ? t->Size : Size);
			t->Model->UpdateLayout();
		}
	}

	for(auto i : VisualSpaces)
	{
		VisualGraph->AddNode(i.Space, t->Model->Visual);
	}

	for(auto i : ActiveSpaces)
	{
		ActiveGraph->AddNode(i.Space, t->Model->Active);
	}

	t->Model->Open(this);
	t->Shown = true;

	if(Header)
	{
		Header->Select(m);

		auto at = Transformation;
		at.Position = {Transformation.Position.x, Transformation.Position.y - (t->Model->Size.H - Header->Transformation.Position.y), Transformation.Position.z};
		Transform(at);
		t->Model->TransformY(0);
		Header->Transform({0, t->Model->Size.H, 0});
	}
}

void CGroupUnit::CloseModel(CUol & m)
{
	auto t = Header->Tabs.Find([&m](auto i){ return i->Model.Url == m; });

	t->Model->Close(this);

	for(auto i : VisualSpaces)
	{
		VisualGraph->RemoveNode(i.Space, t->Model->Visual);
	}

	for(auto i : ActiveSpaces)
	{
		ActiveGraph->RemoveNode(i.Space, t->Model->Active);
	}

	t->Shown = false;
}

void CGroupUnit::Open()
{
	for(auto i : Header->Tabs)
		if(i->Shown)
			i->Model->Open(this);	
}

void CGroupUnit::Close()
{
	for(auto i : Header->Tabs)
		if(i->Shown)
			i->Model->Close(this);	
}

CSpaceBinding<CVisualSpace> & CGroupUnit::AllocateVisualSpace(CViewport * vp)
{
	auto & s = __super::AllocateVisualSpace(vp);
	
	for(auto i : Header->Tabs)
	{
		if(i->Shown)
		{
			VisualGraph->AddNode(s.Space, i->Model->Visual);
		}
	}
	return s;
}

void CGroupUnit::DeallocateVisualSpace(CVisualSpace * s)
{
	for(auto i : Header->Tabs)
	{
		if(i->Shown)
		{
			VisualGraph->RemoveNode(s, i->Model->Visual);
		}
	}
	__super::DeallocateVisualSpace(s);
}

CSpaceBinding<CActiveSpace> & CGroupUnit::AllocateActiveSpace(CViewport * vp)
{
	auto & s = __super::AllocateActiveSpace(vp);
	
	for(auto i : Header->Tabs)
	{
		if(i->Shown)
		{
			ActiveGraph->AddNode(s.Space, i->Model->Active);
		}
	}
	return s;
}

void CGroupUnit::DeallocateActiveSpace(CActiveSpace * s)
{
	for(auto i : Header->Tabs)
	{
		if(i->Shown)
		{
			ActiveGraph->RemoveNode(s, i->Model->Active);
		}
	}
	__super::DeallocateActiveSpace(s);
}

CSize CGroupUnit::DetermineSize(CSize & smax, CSize & s)
{
	Size = CSize::Empty;

	for(auto i : Header->Tabs)
		if(i->Shown)
		{
			i->Model->DetermineSize(smax, i->Size);
			Size.W = max(Size.W, i->Model->Size.W);
			Size.H = max(Size.H, i->Model->Size.H);
		}
	
	return Size;
}

CTransformation CGroupUnit::DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t)
{
	return t;
}

EPreferedPlacement CGroupUnit::GetPreferedPlacement()
{
	return EPreferedPlacement::Default;
}

void CGroupUnit::Update()
{
	for(auto i : Header->Tabs)
		if(i->Shown)
			i->Model->UpdateLayout();
	
	if(Header)
	{
		Header->Transform({0, Size.H, 0});
		Header->Update(Size);
	}
}

CCamera * CGroupUnit::GetPreferedCamera()
{
	return null;
}

CSize CGroupUnit::Measure()
{
	return Size;
}

void CGroupUnit::Normalize()
{
	for(auto i : Header->Tabs)
		if(i->Shown)
		{	
			Transform(Transformation * i->Model->Transformation);
			i->Model->Transform(CTransformation::Identity); // move model transformation to area transformation, needed to resolve resizing issues

			if(Header)
			{
				Header->Transform({0, i->Model->Size.H, 0});
			}
		}
}

void CGroupUnit::Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin)
{
	auto t = Header->Tabs.Find([](auto i){ return i->Shown; });
	ActiveGraph->Activate(t->Model->Active, null);
}

void CGroupUnit::Interact(bool e)
{
	auto t = Header->Tabs.Find([](auto i){ return i->Shown; });
	t->Model->Active->IsPropagator = e;
}

bool CGroupUnit::ContainsEntity(CUol & o)
{
	return Entity.Url == o || Header->Tabs.Has([&o](auto i){ return CUol(i->Model.Url.Parameters(L"entity")) == o; });
}

bool CGroupUnit::ContainsAvatar(CUol & o)
{
	return Header->Tabs.Has([&o](auto i){ return i->Model.Url == o; });
}

CString & CGroupUnit::GetDefaultInteractiveMasterTag()
{
	return Entity->DefaultInteractiveMasterTag;
}
