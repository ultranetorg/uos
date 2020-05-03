#include "stdafx.h"
#include "RectangleMenu.h"

using namespace uos;

CRectangleMenu::CRectangleMenu(CWorld * w, CStyle * st, const CString & name) : CModel(w, w->Server, ELifespan::Visibility, name)
{
	World = w;
	Style = st;
	Tags = {L"all"};
	
	
	PreferedPlacement[AREA_TOP] = EPreferedPlacement::Exact;

	Active->IsListener = true;
	Active->StateChanged	+= ThisHandler(OnStateModified);
	Active->MouseInput		+= ThisHandler(OnMouse);

	Visual->Transformating = ETransformating::Root;
	Active->Transformating = ETransformating::Root;

	Express(L"W", []{ return 0.f; });
	Express(L"H", []{ return 0.f; });

	auto s = new CRectangleMenuSection(World, Style);
	s->Express(L"B", []{ return CFloat6(1.f); });
	//s->SetMaterial(L"0 0 0");
	//s->Visual->SetShader(Level->Engine->PipelineFactory->DiffuseColorShader);
	SetSection(s);
	s->Free();
}

CRectangleMenu::~CRectangleMenu()
{
	if(Section)
	{
		if(Section->IsOpen())
			Section->Close();
		Section->Free();
	}

	Active->StateChanged	-= ThisHandler(OnStateModified);
	Active->MouseInput		-= ThisHandler(OnMouse);
}

void CRectangleMenu::OnMouse(CActive * r, CActive * s, CMouseArgs * a)
{
	if(a->Control == EControl::LeftButton || a->Control == EControl::MiddleButton)
	{
		if(a->Event == EGraphEvent::Click)
		{
			auto mi = s->AncestorOwnerOf<IMenuItem>();
		
			if(mi) // leaf item
			{
				Close();
	
				Clicked(mi, a->Pick);
		
				if(mi->Clicked)
				{
					mi->Clicked(a, mi);
				}
			}

			a->StopPropagation = true;
		}
	}
}

void CRectangleMenu::SetSection(IMenuSection * m)
{
	Section = sh_assign(Section, dynamic_cast<CRectangleMenuSection *>(m));
}

void CRectangleMenu::Close(CUnit * a)
{
	Section->Close();

	if(Parent)
	{
		Parent->RemoveNode(this);
	}
}

void CRectangleMenu::DetermineSize(CSize & smax, CSize & s)
{
	UpdateLayout(CLimits(CSize::Empty, smax), false);
}

CTransformation CRectangleMenu::DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t)
{
	return ps->GetPoint(pk.Camera->Viewport, pk.Vpp).VertexTransform(ps->GetMatrix(pk.Camera->Viewport));
}

void CRectangleMenu::OnStateModified(CActive * r, CActive * s, CActiveStateArgs * a)
{
	if(Unit && s == Active && a->Old == EActiveState::Active/* && Parent != null*/)
	{
		Close();
	}
}

IMenuSection * CRectangleMenu::CreateSection(const CString & name)
{
	auto s = new CRectangleMenuSection(World, Style, name);
	s->Express(L"B", []{ return CFloat6(1.f); });
	//s->SetMaterial(L"0 0 0");
	//s->Visual->SetShader(Level->Engine->PipelineFactory->DiffuseColorShader);

	return s;
}

void CRectangleMenu::Open(CPick & pick, CSize const & size)
{
	auto sf = new CShowParameters(pick);

	Area = World->AllocateUnit(this);
	World->Show(Area, AREA_TOP, sf);

	//auto s = pick.Camera->ProjectVertex(pick.GetWorldPosition());
	//auto w = pick.Camera->Viewport->W;
	//auto h = pick.Camera->Viewport->H;

	Section->Open(Area, this, &pick, 0, size.W, size.H, 0);

	sf->Free();
}

void CRectangleMenu::Close()
{
	if(Area) // means opened
	{
		World->Hide(Area, null);
		Area = null;
	}
}
