#include "StdAfx.h"
#include "Text.h"

using namespace uos;

CText::CText(CWorldLevel * l, CStyle * s, CString const & name, bool active)  : CElement(l, name)
{
	Style	= s;
	Engine	= l->Engine;
	
	UseCanvas(Visual, null, Level->Engine->PipelineFactory->TextShader);

	if(active)
	{
		Active->SetMesh(Visual->Mesh);
	}
	
	SetFont(Level->Engine->FontFactory->GetFont(s->Get<CFontDefinition>(L"Text/Font")));
	SetWrap(false);
	SetColor(s->Get<CFloat4>(L"Text/Color/Normal"));

	Express(L"C",	[this](auto apply)
					{
						auto l = Climits;
							
						auto w = l.Smax.W > 0 ? l.Smax.W : FLT_MAX;
						auto h = l.Smax.H > 0 ? l.Smax.H : FLT_MAX;
							
						return Measure(w, h);
					});
}

CText::~CText()
{
}

void CText::UpdateLayout(CLimits const & l, bool apply)
{
	__super::UpdateLayout(l, apply);

	UpdateCanvas();
}

void CText::Draw()
{
	auto c = Texture->BeginDraw();
	c->Clear(CFloat4(0));
	c->DrawText(Text, Font, CFloat4(1), CRect(0, 0, c->Size.W, c->Size.H), XAlign, YAlign, Wrap, Ellipsis);
	Texture->EndDraw();
}

CSize CText::Measure(float wmax, float hmax)
{
	return Font->Measure(Text, wmax, hmax, Wrap, Ellipsis, TabLength, XAlign, YAlign);
}
	
void CText::SetFont(CFont * f)
{
	if(Font != f)
	{
		Font = f;
		UpdateCanvas();
	}
}

void CText::SetText(const CString & t)
{
	Text = t;
	UpdateCanvas();
}

void CText::SetWrap(bool e)
{
	Wrap = e;
	UpdateCanvas();
}

void CText::SetColor(CFloat4 & c)
{
	Color = c;
	Visual->Material->Float4s[L"Color"] = Color;
}

int CText::GetPosition(const CString & s, float x)
{
	int w = 0;
	unsigned int i = 0;
	LONG c;

	auto t = TabLength * Font->TextMetric.tmAveCharWidth;

	for(; i < s.size(); i++)
	{	
		if(s[i] == L'\t' && t > 0)
			if(w % t == 0)
				c = t;
			else
				c = t - w % t;
		else
			c = Font->Widths[s[i]];

		if(w <= x && x <= w + c )
		{
			if(x - w < c * 0.5f)
			{
				break;
			}
			else
			{
				i++;
				break;
			}
		}
		w += c;
	}

	return i;
}

void CText::LoadProperties(CStyle * s, CXon * n)
{
	__super::LoadProperties(s, n);

	for(auto i : n->Children)
	{
		if(i->Name == L"Text")
		{
			SetText(i->Get<CString>());
		}
	}
}

void CText::SetEntity(IEntity * e)
{
	Entity = static_cast<IStringEntity *>(e);
	SetText(Entity->GetValue());
}

CElement * CText::Clone()
{
	return new CText(Level, Style, Name, !!Active->Mesh);
}
