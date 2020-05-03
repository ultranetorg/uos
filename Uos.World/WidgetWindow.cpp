#include "stdafx.h"
#include "WidgetWindow.h"

using namespace uos;

CWidgetWindow::CWidgetWindow(CWorld * l, CServer * srv, CStyle * s, const CString & name) : CModel(l, srv, ELifespan::Permanent, name), Sizer(l)
{
	World = l;
	Style = s;

	Active->IsListener = true;
	Active->MouseInput += ThisHandler(OnMouse);
}

CWidgetWindow::~CWidgetWindow()
{
	if(Menu)
	{
		Menu->Close();
		Menu->Free();
	}

	RemoveNode(Face);
	Sizer.SetTarget(null);

	OnDependencyDestroying(Entity);
}

void CWidgetWindow::SetFace(CElement * e)
{
	Face = e;

	Face->Express(L"W", [this]{ return IW; });
	Face->Express(L"H", [this]{ return IH; });

	AddNode(e);
}

void CWidgetWindow::SetEntity(CUol & o)
{
	Entity = Server->FindObject(o);
	Entity->Destroying += ThisHandler(OnDependencyDestroying);
}

void CWidgetWindow::OnDependencyDestroying(CNexusObject * o)
{
	if(Entity && o == Entity)
	{
		Entity->Destroying -= ThisHandler(OnDependencyDestroying);
		Entity.Clear();
	}
}

void CWidgetWindow::SaveInstance()
{
	CTonDocument d;

	d.Add(L"Size")->Set(Size);

	SaveGlobal(d, GetClassName() + L".xon");
}

void CWidgetWindow::LoadInstance()
{
	CTonDocument d;
	LoadGlobal(d, GetClassName() + L".xon");
	
	auto s = d.Get<CSize>(L"Size");
	Express(L"W", [s]{ return s.W; });
	Express(L"H", [s]{ return s.H; });
}

void CWidgetWindow::Place(IFieldOperations * fo)
{
	__super::Place(fo);

	Sizer.SetTarget(this);
	Sizer.SetGripper(Face);

	Sizer.InGripper =	[this](auto & is)
						{
							return Sizer.InRightBottomCorner(Face, is);
						};
	Sizer.Captured =	[this](auto & p)
						{
							return Operations->GetPositioning(p);
						};
	Sizer.Resizing =	[this](auto & r)
						{
							Express(L"W", [r]{ return r.Size.W; });
							Express(L"H", [r]{ return r.Size.H; });
							PropagateLayoutChanges(Face);
						};
	Sizer.Adjusting =	[this](CResizing & r)
						{
							Operations->MoveAvatar(this, Parent->Transformation * CTransformation(r.PositionDelta));
						};
}

void CWidgetWindow::OnMouse(CActive *, CActive *, CMouseArgs * arg)
{
	if(arg->Control == EControl::RightButton && arg->Event == EGraphEvent::Click)
	{
		if(!Menu)
		{
			Menu = new CRectangleMenu(World, Level->Style, L"Menu");
	
			if(Operations)
			{
				Operations->AddTitleMenu(Menu->Section, this);
				Menu->Section->AddSeparator();
			
				Menu->Section->AddItem(L"Delete")->Clicked = [this](auto, auto){ Operations->DeleteAvatar(this); };
				Menu->Section->AddSeparator();
				Menu->Section->AddItem(L"Properties...");
			}
		}
		
		Menu->Open(arg->Pick);
	
		arg->StopPropagation = true;
	}

	if(arg->Control == EControl::LeftButton && arg->Event == EGraphEvent::Click)
	{
		arg->StopPropagation = true;
	}
}

void CWidgetWindow::DetermineSize(CSize & smax, CSize & s)
{
	__super::DetermineSize(smax, s);
}
