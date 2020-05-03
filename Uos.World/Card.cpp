#include "StdAfx.h"
#include "AvatarCard.h"

using namespace uos;

CCard::CCard(CWorld * l, const CString & name) : CRectangle(l, name)
{
	Level = l;

	TextColor = Level->Style->Get<CFloat4>(L"Text/Color/Normal");

	Express(L"C",	[this](auto apply)
					{
						if(Face)
							Face->UpdateLayout(Climits, apply);

						if(Text)
							Text->UpdateLayout(Climits, apply);

						if(Face && Text)
						{
							if(TitleMode == ECardTitleMode::Right || TitleMode == ECardTitleMode::Left)
							{
								if(TitleMode == ECardTitleMode::Right)
								{
									Face->Transform(0, 0, Z_STEP);
									Text->Transform(Face->W + Spacing, 0, Z_STEP);
								}
								if(TitleMode == ECardTitleMode::Left)
								{
									Face->Transform(Text->W + Spacing, 0, Z_STEP);
									Text->Transform(0, 0, Z_STEP);
								}
								Text->TransformY((Face->H - Text->H)/2);
							}
							if(TitleMode == ECardTitleMode::Bottom)
							{
								Face->Transform(0, Text->H + Spacing, Z_STEP); 
								Text->Transform(0, 0, Z_STEP); 
							}
							if(TitleMode == ECardTitleMode::Top)
							{
								Text->Transform(0, Face->H + Spacing, Z_STEP); 
								Face->Transform(0, 0, Z_STEP); 
							}
						}
						else if(!Face && Text)
							Text->Transform(0, 0, Z_STEP);
						else if(Face && !Text)
							Face->Transform(0, 0, Z_STEP);

						auto bb = CAABB::InversedMax;

						if(Face)
							bb.Join2D(Face->Transformation, Face->Size);

						if(Text)
							bb.Join2D(Text->Transformation, Text->Size);

						return bb.GetSize();

					});
}

CCard::~CCard()
{
	if(Text)
	{
		RemoveNode(Text);
		Text->Free();
	}

	if(Face)
	{
		SetFace(null);
	}
}

void CCard::SetFace(CElement * f)
{
	if(f)
	{
		Face = f;
		AddNode(Face);
	} 
	else
	{
		RemoveNode(Face);
		Face = null;
	}
}

void CCard::SetTitleMode(ECardTitleMode tm)
{
	TitleMode = tm;

	if(tm != ECardTitleMode::No)
	{
		if(!Text)
		{
			Text = new CText(Level, Level->Style, L"Title", true);
			Text->SetText(Title);
			Text->SetWrap(true);
			Text->SetColor(TextColor);
			Text->Shadow = true;
			Text->Wrap = true;
			AddNode(Text);
		}

		if(Face)
		{
			if(Type == AVATAR_ICON2D)
			{
				if(TitleMode == ECardTitleMode::Bottom || TitleMode == ECardTitleMode::Top)
				{
					Text->Express(L"W", [this]{ return Face->W; });
					Text->Express(L"H", [this]{ return min(Metrics.TextSize.H, Text->IHtoH(Text->C.H)); });
				}
				else
				{
					Text->Express(L"W", [this]{ return min(Metrics.TextSize.W, Text->IWtoW(Text->C.W)); });
					Text->Express(L"H", [this]{ return min(Text->Measure(Metrics.TextSize.W, FLT_MAX).H/*n->CH*/, Face->H); });
				}
			}
	
			if(Type == AVATAR_WIDGET)
			{	
				if(TitleMode == ECardTitleMode::Bottom || TitleMode == ECardTitleMode::Top)
				{
					Text->Express(L"W", [this]{ return min(Text->IWtoW(Text->C.W), Face->W); });
					Text->Express(L"H", [this]{ return min(Metrics.TextSize.H, Text->C.H); });
				}
				else
				{
					Text->Express(L"W", [this]{ return min(Metrics.TextSize.W, Text->IWtoW(Text->C.W)); });
					Text->Express(L"H", [this]{ return min(Text->Measure(Metrics.TextSize.W, FLT_MAX).H/*n->CH*/, Face->H); });
				}
			}
		} 
		else
		{
			Text->Express(L"H", [this]{ return min(Metrics.TextSize.H, Text->IHtoH(Text->C.H)); });
			Text->Express(L"W", [this]{ return min(Metrics.TextSize.W, Text->IWtoW(Text->C.W)); });
		}
	}
	else if(Text && tm == ECardTitleMode::No)
	{
		RemoveNode(Text);
		Text->Free();
		Text = null;
	}
	
	if(TitleMode == ECardTitleMode::Right)
	{
		Text->XAlign = EXAlign::Left;
		Text->YAlign = EYAlign::Center;
	}
	if(TitleMode == ECardTitleMode::Left)
	{
		Text->XAlign = EXAlign::Right;
		Text->YAlign = EYAlign::Center;
	}
	else if(TitleMode == ECardTitleMode::Bottom)
	{
		Text->XAlign = EXAlign::Center;
		Text->YAlign = EYAlign::Top;
	}
	else if(TitleMode == ECardTitleMode::Top)
	{
		Text->XAlign = EXAlign::Center;
		Text->YAlign = EYAlign::Bottom;
	}

	if(Text)
	{
		Text->SetText(Title);
	}
}
