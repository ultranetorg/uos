#include "stdafx.h"
#include "HighspaceGroupUnit.h"
#include "WorldServer.h"

using namespace uos;

CHighspaceGroupUnit::CHighspaceGroupUnit(CWorldServer * l, CString & dir, CUol & entity, CString const & type, CView * hview, CVisualGraph * vg, CActiveGraph * ag) : CGroupUnit(l, dir, entity, type, hview, vg, ag)
{
	Initialize();
	OpenModel(Header->Tabs.front()->Model.Url);
}

CHighspaceGroupUnit::CHighspaceGroupUnit(CWorldServer * l, CString & dir, CString const & name, CView * hview, CVisualGraph * vg, CActiveGraph * ag) : CGroupUnit(l, dir, name, hview, vg, ag)
{
	Initialize();
	Load();
}

CHighspaceGroupUnit::~CHighspaceGroupUnit()
{
	Remove(Header);
}

void CHighspaceGroupUnit::Initialize()
{
	Header->TitleMode			= ECardTitleMode::Right;
	Header->IconSize			= {24, 24, 0};
	Header->Stack->Direction	= EDirection::X;
	Header->Stack->XAlign		= EXAlign::Left;
	Header->Stack->YAlign		= EYAlign::Center;

	Header->Selected += [this](auto m){ OpenModel(m); };

	Add(Header, EAddLocation::Front);
}

void CHighspaceGroupUnit::Save(CXon * x)
{
	__super::Save(x);

	x->Add(L"Entity")->Set(Entity.Url);

	for(auto i : Header->Tabs)
	{
		if(i->Shown)
			x->Add(L"Open")->Set(i->Model.Url);
	}

	for(auto i : Header->Tabs)
	{
		auto m = x->Add(L"Model");
		m->Set(i->Model.Url);
		m->Add(L"Size")->Set(i->Model ? i->Model->Size : i->Size);
	}
}

void CHighspaceGroupUnit::Load(CXon * x)
{
	__super::Load(x);

	Entity = Level->FindObject(x->Get<CUol>(L"Entity"))->As<CGroup>();
		
	for(auto i : x->Many(L"Model"))
	{
		auto t = AddModel(i->Get<CUol>(), null);
		t->Size = i->Get<CSize>(L"Size");
	}

	for(auto i : x->Many(L"Open"))
	{
		OpenModel(i->Get<CUol>());
	}
}
