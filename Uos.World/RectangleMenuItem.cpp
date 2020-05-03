#include "stdafx.h"
#include "RectangleMenuItem.h"

using namespace uos;

/// CStandardMenuItem

CRectangleMenuItem::CRectangleMenuItem(CWorldLevel * l, CStyle * s, const CString & name) : CRectangle(l, name)
{
	Style = s;
}

/// CRectangleTextMenuItem

CRectangleTextMenuItem::CRectangleTextMenuItem(CWorldLevel * l, CStyle * s, CTexture * icon, CString const & text, const CString & name) : CRectangleMenuItem(l, s, name)
{
	Icon = new CRectangle(Level, L"Icon");
	Icon->Express(L"IW", []{ return 16.f; });
	Icon->Express(L"IH", []{ return 16.f; });
	AddNode(Icon);
	Icon->Free();
	
	Label = new CText(Level, Style);
	Label->SetText(text);
	Label->Express(L"M.RT", [this]{ return 10.f; });
	AddNode(Label);
	Label->Free();

	//Express(L"P.RT", []{ return 40.f; });
	Express(L"C",	[this](auto apply)
					{
						auto h = 16.f;
						auto w = 0.f;
						auto s = Climits.Smax;

						if(Icon)
						{
							Icon->UpdateLayout(CLimits(s, s), apply);
							h = max(h, Icon->Size.H);
							w += Icon->Size.W;
						}
						else
							w += 16;

						if(Arrow)
						{
							Arrow->UpdateLayout(CLimits(s, s), apply);
							h = max(h, Arrow->Size.H);
							w += max(16, Arrow->Size.W);
						}
						else
							w += 16;

						if(Label)
						{
							s.W -= w;
							Label->UpdateLayout(CLimits(s, s), apply);
							h = max(h, Label->Size.H);
							w += Label->Size.W;
						}
						else
							w += 0;

						for(auto i : Nodes)
						{
							i->Transform(0, (h - i->Size.H)/2, Z_STEP);
						}

						if(Icon)
							Icon->TransformX(0);

						if(Label)
							Label->TransformX(Icon->W);

						if(Arrow)
							Arrow->TransformX(Climits.Smax.W - 16 + (16 - Arrow->Size.W)/2);

						return CSize(w, h, 0);
					});
}

CRectangleTextMenuItem::~CRectangleTextMenuItem()
{
	if(Icon)
		RemoveNode(Icon);

	RemoveNode(Label);
}

void CRectangleTextMenuItem::Enable(bool e)
{
	Active->Enable(e);

	if(e)
	{
		Label->SetColor(Style->Get<CFloat4>(L"Menu/Text/NormalColor"));
	} 
	else
	{
		Label->SetColor(Style->Get<CFloat4>(L"Menu/Text/DisabledColor"));
	}
}

void CRectangleTextMenuItem::Highlight(CArea * a, bool e, CSize & s, CPick * p)
{
	Area = a;

	if(Enabled)
	{
		if(e)
		{
			Label->SetColor(Style->Get<CFloat4>(L"Menu/Text/HighlightedColor"));
		}
		else
		{
			Label->SetColor(Style->Get<CFloat4>(L"Menu/Text/NormalColor"));
		}
	} 
}

/// SeparatorMenuItem

CRectangleSeparatorMenuItem::CRectangleSeparatorMenuItem(CWorldLevel * w, CStyle * s) : CRectangleMenuItem(w, s, GetClassName())
{
	Style = s;
	Expressions.Clear();
	//Express(L"EElementField::B", [this](auto n){ Border = 1; });
	Express(L"M", []{ return CFloat6(4.f); });
	Express(L"H", []{ return 4.f * 2 + 2; });

	//SetupContent();
	Visual->SetMaterial(Style->GetMaterial(L"Menu/Separator/Color"));
}
