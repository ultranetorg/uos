#include "StdAfx.h"
#include "Button.h"

using namespace uos;

CButton::CButton(CWorldLevel * l, CStyle * s, CString const & name) : CRectangle(l, name)
{
	Style = s;

	Express(L"B",	[]{ return CFloat6(1.f); });
	Express(L"P",	[]{ return CFloat6(10, 10, 5, 5, 0, 0); });
	Express(L"C",	[this](auto apply)
					{
						Text->UpdateLayout(Climits, apply);
						return Text->Size;
					});

	Text = new CText(l, s);

	Text->SetColor(s->Get<CFloat4>(L"Button/Text/Normal/Color"));
	BorderMaterial = s->GetMaterial(L"Button/Border/Normal/Material");
	Visual->SetMaterial(s->GetMaterial(L"Button/Background/Normal/Material"));

	//Text->XAlign = EXAlign::Center;
	//Text->YAlign = EYAlign::Center;
	//Text->Express(L"EElementField::W", [](auto n){ n->W = n->MaxArea.Width; });
	//Text->Express(L"EElementField::H", [](auto n){ n->H = n->MaxArea.Height; });
	AddNode(Text);
	Text->Free();

	Active->Listen(true);
	Active->MouseInput += ThisHandler(OnMouse);
}

CButton::~CButton()
{
}

void CButton::SetText(CString const & t)
{
	Text->SetText(t);
}

void CButton::OnMouse(CActive * r, CActive * s, CMouseArgs * a)
{
	if(a->Event == EGraphEvent::Enter)
	{
		Text->SetColor(Style->Get<CFloat4>(L"Button/Text/Highlighted/Color"));
		VBorder->SetMaterial(Style->GetMaterial(L"Button/Border/Highlighted/Material"));
		Visual->SetMaterial(Style->GetMaterial(L"Button/Background/Highlighted/Material"));
	}
	if(a->Event == EGraphEvent::Leave)
	{
		Text->SetColor(Style->Get<CFloat4>(L"Button/Text/Normal/Color"));
		VBorder->SetMaterial(Style->GetMaterial(L"Button/Border/Normal/Material"));
		Visual->SetMaterial(Style->GetMaterial(L"Button/Background/Normal/Material"));
	}

	if(a->Control == EControl::LeftButton)
	{
		if(a->Action == EInputAction::On)
		{	
			Text->SetColor(Style->Get<CFloat4>(L"Button/Text/Pressed/Color"));
			VBorder->SetMaterial(Style->GetMaterial(L"Button/Border/Pressed/Material"));
			Visual->SetMaterial(Style->GetMaterial(L"Button/Background/Pressed/Material"));
		}
		if(a->Action == EInputAction::Off)
		{	
			Text->SetColor(Style->Get<CFloat4>(L"Button/Text/Highlighted/Color"));
			VBorder->SetMaterial(Style->GetMaterial(L"Button/Border/Highlighted/Material"));
			Visual->SetMaterial(Style->GetMaterial(L"Button/Background/Highlighted/Material"));
		}
		if(a->Event == EGraphEvent::Click)
		{
			Pressed(this);
			Text->SetColor(Style->Get<CFloat4>(L"Button/Text/Normal/Color"));
			VBorder->SetMaterial(Style->GetMaterial(L"Button/Border/Normal/Material"));
			Visual->SetMaterial(Style->GetMaterial(L"Button/Background/Normal/Material"));
		}
	}
}

void CButton::UpdateLayout(CLimits const & l, bool apply)
{
	__super::UpdateLayout(l, apply);

	Text->Transform((IW - Text->W)/2, (IH - Text->H)/2, Z_STEP);
}

void CButton::LoadProperties(CStyle * s, CXon * n)
{
	__super::LoadProperties(s, n);

	for(auto i : n->Children)
	{
		if(i->Name == L"Text")
		{
			Text->SetText(i->Get<CString>());
		}
	}
}
