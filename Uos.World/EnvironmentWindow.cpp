#include "StdAfx.h"
#include "EnvironmentWindow.h"

using namespace uos;

CEnvironmentWindow::CEnvironmentWindow(CWorld * l, CServer * server, CStyle * s, const CString & name) : CModel(l, server, ELifespan::Permanent, name)
{
	Level = l;
	Server = server;
	Style = s;
	UseHeader = true;

	ServiceMenu = null;

	Active->ActivatePassOn = true;
}
	
CEnvironmentWindow::~CEnvironmentWindow()
{
	if(ServiceMenu) 
		ServiceMenu->Free();

	RemoveNode(Content);

	if(Sizer)
	{
		delete Sizer;
	}

	OnDependencyDestroying(Entity);
}

void CEnvironmentWindow::SetEntity(CUol & e)
{
	Entity = Server->FindObject(e);
	Entity->Destroying += ThisHandler(OnDependencyDestroying);
}

void CEnvironmentWindow::OnDependencyDestroying(CNexusObject * o)
{
	if(Entity && o == Entity)
	{
		Entity->Destroying -= ThisHandler(OnDependencyDestroying);
		Entity.Clear();
	}
}

void CEnvironmentWindow::SetContent(CElement * n)
{
	if(Content)
	{
		throw CException(HERE, L"Content already set");
	}

	Content = n;
	AddNode(n);

	Express(L"IW",	[this]{ return Content->W; });
	Express(L"IH",	[this]{ return Content->H; });
}
	
void CEnvironmentWindow::OnMouse(CActive * r, CActive * s, CMouseArgs * a)
{
	if(a->Class == EInputClass::Mouse && a->Event == EGraphEvent::Click)
	{
		if(a->Control == EControl::LeftButton)
		{
			Level->Hide(Unit, null);
		}
	
		if(a->Control == EControl::RightButton)
		{
			if(!ServiceMenu)
			{
				ServiceMenu = new CRectangleMenu(Level, Style, L"WindowMenu");
				ServiceMenu->Section->AddItem(L"Hide")->Clicked =	[this](auto, auto){ Level->Hide(Unit, null); };
			}
				
			ServiceMenu->Open(a->Pick);
		}
	}
}

void CEnvironmentWindow::OnIconCursorMoved(CActive * r, CActive * s, CMouseArgs * a)
{
	if(a->Event == EGraphEvent::Captured && a->Capture.Message.Control == EControl::RightButton)
	{
		auto u = Url;
		Level->Drag(CArray<CDragItem>{CDragItem(u, (CUrl)Unit->Entity.Url)});
	}
}

void CEnvironmentWindow::LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load)
{
	for(auto i : n->Children)
	{
		auto c = load(i, null);
		SetContent(c);
		c->Free();
	}
}

void CEnvironmentWindow::DetermineSize(CSize & smax, CSize & s)
{
	//if(s.IsReal())
	//{
	//	Express(L"Size", [s](auto){ return s; });
	//}
	//UpdateLayout(CLimits::Empty, false);

	throw CException(HERE, L"Inheritors must implement");
}

CTransformation CEnvironmentWindow::DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t)
{
	return t;
}

void CEnvironmentWindow::LoadProperties(CStyle * s, CXon * n)
{
	//__super::LoadProperties(s, n);

	for(auto i : n->Children)
	{
		if(i->Name == L"Sizer")
		{
			UseSizer(this, One(i->Value->As<CXon>()->Get<CString>(L"Gripper")));
		}
	}

	if(Sizer)
	{
		UpdateLayout(CLimits::Empty, false);
		Sizer->Min = Size;
	}
}

void CEnvironmentWindow::UseSizer(CElement * target, CElement * gripper)
{
	Sizer = new CRectangleSizer(Level);
	Sizer->SetTarget(target);
	Sizer->SetGripper(gripper);
			
	Sizer->InGripper =	[this](auto & is)
						{
							return Sizer->InRightBottomCorner(Content, is);
						};
	Sizer->Captured =	[this](auto & pk)
						{
							return Unit->AncestorOf<CPositioningArea>()->Positioning;
						};
	Sizer->Resizing =	[this](CResizing & r)
						{
							Express(L"W", [r]{ return r.Size.W; });
							Express(L"H", [r]{ return r.Size.H; });
							UpdateLayout();
						};
	Sizer->Adjusting =	[this](CResizing & r)
						{
							Transform(Transformation * CTransformation(r.PositionDelta));
							GetUnit()->Normalize();
						};

}