#include "stdafx.h"
#include "Header.h"
#include "WorldServer.h"
#include "AvatarCard.h"

using namespace uos;

CHeaderTab::CHeaderTab(CWorldServer * l, CUol & m, CModel * model)
{
	Level = l;
	Model.Url = m;
	Model.Object = model;

	auto a = l->GenerateAvatar(CUol(m.Parameters(L"entity")), AVATAR_ICON2D);
	Card = new CAvatarCard(l);
	Card->SetAvatar(a, L"");
	Card->SetEntity(CUol(m.Parameters(L"entity")));
	Card->ApplyStyles(Level->Style, {L"Header/Normal"});
}

CHeaderTab::~CHeaderTab()
{
	 Card->Free();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CHeader::CHeader(CWorldServer * l, CVisualGraph * vg, CActiveGraph * ag, CView * view) : CArea(l, GetClassName())
{
	View = view;

	Stack = new CStack(l, l->Style, L"header");
	Stack->Spacing = 10;
	Stack->Direction = EDirection::X;
	Stack->XAlign = EXAlign::Left;
	Stack->YAlign = EYAlign::Center;
	Stack->ApplyStyles(Level->Style, {L"Header"});
	Stack->Active->IsPropagator = false;
	Stack->Active->Listen(true);
	Stack->Active->MouseInput +=	[this](auto r, CActive * s, auto arg)
									{
										auto icon = s->AncestorOwnerOf<CAvatarCard>();

										if(icon)
										{
											auto t = Tabs.Find([icon](auto i){ return i->Card == icon; });
		
											if(arg->Event == EGraphEvent::Click && arg->Control == EControl::LeftButton)
											{
												Selected(t->Model.Url);
											}
	
											arg->StopPropagation = true;
										}
									};


	VisualGraph = vg;
	ActiveGraph = ag;
	ActiveGraph->Root->AddNode(Stack->Active);
}

CHeader::~CHeader()
{
	ActiveGraph->Root->RemoveNode(Stack->Active);

	Stack->Free();

	for(auto i : Tabs)
	{
		delete i;
	}
}

void CHeader::Select(CUol & m)
{
	auto t = Tabs.Find([m](auto i){ return i->Model.Url == m; });

	for(auto i : Tabs)
	{
		if(i->Selected)
		{
			i->Card->ApplyStyles(Level->Style, {L"Header/Normal"});
		}
	}
	
	t->Selected = true;
	t->Card->ApplyStyles(Level->Style, {L"Header/Selected"});
	
	Update(Size);
}

CHeaderTab * CHeader::AddTab(CUol & m, CModel * model)
{
	auto t = new CHeaderTab(Level, m, model);

	Tabs.push_back(t);
	Stack->AddNode(t->Card);

	return t;
}

CSpaceBinding<CVisualSpace> & CHeader::AllocateVisualSpace(CViewport * vp)
{
	auto & s = __super::AllocateVisualSpace(vp);

	s.Space->AddGraph(VisualGraph);// graphs to space
	VisualGraph->AddNode(s.Space, Stack->Visual); // nodes to graphs

	return s;
}

CSpaceBinding<CActiveSpace> & CHeader::AllocateActiveSpace(CViewport * vp)
{
	auto & s = __super::AllocateActiveSpace(vp);

	s.Space->AddGraph(ActiveGraph);
	ActiveGraph->AddNode(s.Space, Stack->Active); // nodes to graphs

	return s;
}

void CHeader::DeallocateVisualSpace(CVisualSpace * s)
{
	s->RemoveGraph(VisualGraph);
	//VisualGraph->RemoveNode(s, Stack->Visual); 
	__super::DeallocateVisualSpace(s);
}

void CHeader::DeallocateActiveSpace(CActiveSpace * s)
{
	s->RemoveGraph(ActiveGraph);
	//ActiveGraph->RemoveNode(s, Stack->Active);
	__super::DeallocateActiveSpace(s);
}

void CHeader::Update(CSize & s)
{
	Size = s;

	Stack->Express(L"W", [s]{ return s.W; });
	
	CAvatarMetrics am;
	am.FaceSize = IconSize;

	if(Stack->Direction == EDirection::X)	am.TextSize = {(s.W - IconSize.W * Tabs.size())/Tabs.size(), IconSize.H, 0}; else
	if(Stack->Direction == EDirection::Y)	am.TextSize = {s.W - IconSize.W, IconSize.H, 0};

	for(auto i : Tabs)
	{
		i->Card->SetMetrics(am);
		i->Card->SetTitleMode(TitleMode);
	}

	Stack->UpdateLayout(CLimits::Max, true);
}
