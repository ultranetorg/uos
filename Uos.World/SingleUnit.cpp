#include "stdafx.h"
#include "SingleUnit.h"
#include "WorldServer.h"

using namespace uos;

CSingleUnit::CSingleUnit(CWorldServer * l, CModel * m, CView * hview, CVisualGraph * vg, CActiveGraph * ag) : CUnit(l, hview, vg, ag, CGuid::Generate64(GetClassName()))
{
	Model = m;
	m->Take();
	Initialize();
}

CSingleUnit::CSingleUnit(CWorldServer * l, CString & dir, CUol & entity, CString const & type, CView * hview, CVisualGraph * vg, CActiveGraph * ag) : CUnit(l, hview, vg, ag, entity.Object)
{
	Directory = dir;
	Model = l->CreateAvatar(l->GenerateAvatar(entity, type), dir)->As<CModel>();
	Entity = Model->Protocol->GetEntity(entity);
	Initialize();
}

CSingleUnit::CSingleUnit(CWorldServer * l, CString & dir, CView * hview, CVisualGraph * vg, CActiveGraph * ag, CString const & name) : CUnit(l, hview, vg, ag, name)
{
	Directory = dir;

	Load();

	auto adir = CPath::Join(Directory, CNativePath::GetSafe(Model.Url.Object));
	Model = l->CreateAvatar(Model.Url, adir)->As<CModel>();
	Entity = Model->Protocol->GetEntity(Entity.Url);
	Initialize();
}

CSingleUnit::~CSingleUnit()
{
	if(Header)
	{
		if(Header->Parent)
			Remove(Header);
	
		Header->Free();
		Header = null;
	}

	if(Model->Active->Parent == ActiveGraph->Root)
		ActiveGraph->Root->RemoveNode(Model->Active);

	Model->Save();
	
	if(Model->Protocol)
		Level->DestroyAvatar(Model);
	else
		Model->Free();
}

void CSingleUnit::Initialize()
{
	Model->Unit = this;

	Lifespan	= Model->Lifespan;
	Tags		= Model->Tags;
		
	if(Model->UseHeader)
	{
		Header = new CHeader(Level, VisualGraph, ActiveGraph, HeaderView);
		Header->Tags = {L"all"};
		Header->TitleMode			= ECardTitleMode::Right;
		Header->IconSize			= {24, 24, 0};
		Header->Stack->Direction	= EDirection::X;
		Header->Stack->XAlign		= EXAlign::Left;
		Header->Stack->YAlign		= EYAlign::Center;

		Header->AddTab(Model.Url, Model);

		Add(Header, EAddLocation::Front);
	}

	// active as root
	if(!Model->Active->Parent)
		ActiveGraph->Root->AddNode(Model->Active);

	if(Lifespan == ELifespan::Permanent)
	{
		auto adir = CPath::Join(Directory, CNativePath::GetSafe(Model.Url.Object));

		Model->SetDirectories(adir);
	}
}

void CSingleUnit::Open()
{
	Model->Open(this);
}

void CSingleUnit::Close()
{
	Model->Close(this);
}

CSpaceBinding<CVisualSpace> & CSingleUnit::AllocateVisualSpace(CViewport * vp)
{
	auto & s = __super::AllocateVisualSpace(vp);
	VisualGraph->AddNode(s.Space, Model->Visual); // nodes to graphs
	return s;
}

CSpaceBinding<CActiveSpace> & CSingleUnit::AllocateActiveSpace(CViewport * vp)
{
	auto & s = __super::AllocateActiveSpace(vp);
	ActiveGraph->AddNode(s.Space, Model->Active);
	return s;
}

void CSingleUnit::DeallocateVisualSpace(CVisualSpace * s)
{
	VisualGraph->RemoveNode(s, Model->Visual);
	__super::DeallocateVisualSpace(s);
}

void CSingleUnit::DeallocateActiveSpace(CActiveSpace * s)
{
	ActiveGraph->RemoveNode(s, Model->Active);
	__super::DeallocateActiveSpace(s);
}

CSize CSingleUnit::DetermineSize(CSize & smax, CSize & s)
{
	Model->DetermineSize(smax, s);
	return Model->Size;
}

CTransformation CSingleUnit::DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t)
{
	return Model->DetermineTransformation(ps, pk, t);
}

EPreferedPlacement CSingleUnit::GetPreferedPlacement()
{
	return Model->GetPreferedPlacement();
}

void CSingleUnit::Update()
{
	Model->UpdateLayout();

	if(Header)
	{
		Header->Transform({0, Model->Size.H, 0});
		Header->Update(Model->Size);
	}
}

CCamera * CSingleUnit::GetPreferedCamera()
{
	auto c = GetActualView()->Cameras.Find([this](auto i){ return i->Viewport->Tags.Has([this](auto j){ return Model->Tags.Contains(j); }); });
	return c; 
}

CSize CSingleUnit::Measure()
{
	return Model->Size;
}

void CSingleUnit::Normalize()
{
	Transform(Transformation * Model->Transformation);
	Model->Transform(CTransformation::Identity); // move model transformation to area transformation, needed to resolve resizing issues

	if(Header)
	{
		Header->Transform({0, Model->Size.H, 0});
	}
}

void CSingleUnit::Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin)
{
	ActiveGraph->Activate(Model->Active, null);
}

void CSingleUnit::Interact(bool e)
{
	Model->Active->IsPropagator = e;
}

bool CSingleUnit::ContainsEntity(CUol & o)
{
	return Entity.Url == o;
}

bool CSingleUnit::ContainsAvatar(CUol & o)
{
	return Model->Url == o;
}

CString & CSingleUnit::GetDefaultInteractiveMasterTag()
{
	return Entity->DefaultInteractiveMasterTag;
}

void CSingleUnit::Save(CXon * x)
{
	__super::Save(x);

	x->Add(L"Entity")->Set(Entity.Url);
	x->Add(L"Model")->Set(Model.Url);
}

void CSingleUnit::Load(CXon * x)
{
	__super::Load(x);

	Entity.Url	= x->Get<CUol>(L"Entity");
	Model.Url	= x->Get<CUol>(L"Model");
}
