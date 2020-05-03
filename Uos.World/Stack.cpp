#include "stdafx.h"
#include "Stack.h"

using namespace uos;

CStack::CStack(CWorldLevel * l, CStyle * s, CString const & name /*= GetClassName()*/) : CRectangle(l, name)
{
	Style = s;
	Express(L"C",	[this](auto apply)
					{
						auto m = Climits;

						auto ws = 0.f;
						auto hs = 0.f;
						if(Direction == EDirection::X) ws = max(0, Spacing * (Nodes.Size() - 1)); else
						if(Direction == EDirection::Y) hs = max(0, Spacing * (Nodes.Size() - 1));

						float x = 0;
						float y = 0;
						float w = 0;
						float h = 0;

						for(auto i : Nodes)
						{
							i->UpdateLayout(CLimits::Empty, false);

							if(Direction == EDirection::X)
							{
								w += i->Size.W;
								h = max(h, i->Size.H);
							}
							else if(Direction == EDirection::Y)
							{
								w = max(w, i->Size.W);
								h += i->Size.H;
							}
						}

						w += ws;
						h += hs;

						auto wmax = max(w, m.Smax.W);
						auto hmax = max(h, m.Smax.H);

						auto f = Direction == EDirection::X ? wmax - w : hmax - h;

						w = h = 0.f;

						for(auto i : Nodes)
						{
							if(Direction == EDirection::X){ m.Smax.W = i->Size.W + f;	m.Smax.H = hmax - hs;		m.Pmax.W = wmax - ws;	m.Pmax.H = hmax - hs; } else
							if(Direction == EDirection::Y){ m.Smax.W = wmax - ws;		m.Smax.H = i->Size.H + f;	m.Pmax.W = wmax - ws;	m.Pmax.H = hmax - hs; }

							auto s0 = i->Size;

							i->UpdateLayout(m, apply);

							if(Direction == EDirection::X)
							{
								w += i->Size.W;
								h = max(h, i->Size.H);
								f -= max(0, i->Size.W - s0.W);
							}
							else if(Direction == EDirection::Y)
							{
								w = max(w, i->Size.W);
								h += i->Size.H;
								f -= max(0, i->Size.H - s0.H);
							}
							f = max(0, f);
						}

						return CSize(w + ws, h + hs, 0.f);
					});
}

CStack::~CStack()
{
}

void CStack::UpdateLayout(CLimits const & l, bool apply)
{
	__super::UpdateLayout(l, apply);

	if(apply)
	{
		Align();
	}
}

void CStack::Align()
{
	float x = 0;
	float y = 0;

	if(Direction == EDirection::X)
	{
		if(XAlign == EXAlign::Left)		x = 0; else
		if(XAlign == EXAlign::Right)	x = IW - C.W; else
		if(XAlign == EXAlign::Center)	x = (IW - C.W)/2;

		//x = CFloat::Clamp(x, 0.f, wmax - w);
	}

	if(Direction == EDirection::Y)
	{
		if(YAlign == EYAlign::Bottom)	y = C.H; else
		if(YAlign == EYAlign::Top)		y = IH; else
		if(YAlign == EYAlign::Center)	y = (IH - C.H)/2 + C.H;

		//y = CFloat::Clamp(y, 0.f, hmax);
	}

	for(auto i : Nodes)
	{
		if(Direction == EDirection::X)
		{
			if(YAlign == EYAlign::Bottom)	y = 0; else
			if(YAlign == EYAlign::Top)		y = IH - i->Size.H; else
			if(YAlign == EYAlign::Center)	y = (IH - i->Size.H)/2;

			i->Transform(x, y, Z_STEP);
			x += i->Size.W;
			x += Spacing;
		}
		else if(Direction == EDirection::Y)
		{
			if(XAlign == EXAlign::Left)		x = 0; else
			if(XAlign == EXAlign::Right)	x = IW - i->Size.W; else
			if(XAlign == EXAlign::Center)	x = (IW - i->Size.W)/2;

			y -= i->Size.H;
			i->Transform(x, y, Z_STEP);
			y -= Spacing;
		}
	}
}

void CStack::LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load)
{
	for(auto i : n->Children)
	{
		auto wn = load(i, null);
		AddNode(wn);
		wn->Free();
	}
}

void CStack::LoadProperties(CStyle * s, CXon * n)
{
	__super::LoadProperties(s, n);

	for(auto i : n->Children)
	{
		if(i->Name == L"Direction")	Direction = ToDirection(i->Get<CString>()); else
		if(i->Name == L"XAlign")	XAlign = ToXAlign(i->Get<CString>()); else
		if(i->Name == L"YAlign")	YAlign = ToYAlign(i->Get<CString>()); else
		if(i->Name == L"Spacing")	Spacing = CFloat(i->Get<CString>());
	}
}
