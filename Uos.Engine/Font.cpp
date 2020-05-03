#include "stdafx.h"
#include "Font.h"

using namespace uos;

CFont::CFont(CDirectSystem * ge, CString const & family, float size, bool bold, bool italic)
{
	GraphicEngine = ge;
	Family = family;
	Size = size;
	IsBold = bold;
	IsItalic = italic;

	///ge->DWrite->CreateTextFormat(family.data(), 
	///								NULL,
	///								DWRITE_FONT_WEIGHT_NORMAL,
	///								DWRITE_FONT_STYLE_NORMAL,
	///								DWRITE_FONT_STRETCH_NORMAL,
	///								size,
	///								L"", //locale
	///								&Format);

	FontDC = CreateCompatibleDC(null);
	SetMapMode(FontDC, MM_TEXT);

	PixelHeight = Size * GraphicEngine->Scaling.y;

	auto dwBold = IsBold ? FW_BOLD : FW_NORMAL;
	auto dwItalic = IsItalic ? TRUE : FALSE;

	HFont = CreateFont(int(PixelHeight), 0, 0, 0, dwBold, dwItalic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, Family.c_str());

	HFontOld = (HFONT)SelectObject(FontDC, HFont);

	GetTextMetrics(FontDC, &TextMetric);
	int n = TextMetric.tmLastChar + 1;
	
	Height = float(TextMetric.tmHeight) / GraphicEngine->Scaling.y;

	Widths.resize(n);
	CArray<int> widths(n);

	auto r = GetFontUnicodeRanges(FontDC, null);
	auto gs = (GLYPHSET *)malloc(r);
	gs->cbThis = r;
	GetFontUnicodeRanges(FontDC, gs);
	GetCharWidth32(FontDC, 0, TextMetric.tmLastChar, widths.data());

	for(int i=0; i < n; i++)
	{
		Widths[i] = float(widths[i]) / GraphicEngine->Scaling.x;
	}

	Widths[L'\t'] = float(TextMetric.tmAveCharWidth) / GraphicEngine->Scaling.x;

	free(gs);
}

CFont::~CFont()
{
	//Format->Release();
	SelectObject(FontDC, HFontOld);
	DeleteObject(HFont);
	DeleteDC(FontDC);
	
	DeleteObject(HFont);
}

CSize CFont::Measure(const CString & t, float wmax, float hmax, bool wrap, bool ells, int tabs, EXAlign xa, EYAlign ya)
{
 	wmax *= GraphicEngine->Scaling.x;
 	hmax *= GraphicEngine->Scaling.y;

	DRAWTEXTPARAMS p = {};
	p.cbSize = sizeof(DRAWTEXTPARAMS);
	p.iTabLength = tabs;

	auto r = CRect(0, 0, wmax, hmax).ToRECT(CSize(wmax, hmax, 0));

	DrawTextEx(FontDC, LPWSTR(t.data()), int(t.size()), &r, DT_CALCRECT | GetDrawFlags(wrap, ells, xa, ya), &p);

	CSize c;
	c.W = float(r.right - r.left)			/ GraphicEngine->Scaling.x;
	c.H = max(r.bottom - r.top, Height)		/ GraphicEngine->Scaling.y;

	return c;
}

///int CFont::Pick(const CString & t, int tabs, float x)
///{
///	DRAWTEXTPARAMS p = {};
///	p.cbSize = sizeof(DRAWTEXTPARAMS);
///	p.iTabLength = tabs;
///
///	RECT r = {0, 0, int(x * GraphicEngine->MaxScaling.x), INT_MAX};
///
///	DrawTextEx(FontDC, LPWSTR(t.data()), int(t.size()), &r, DT_NOPREFIX | DT_EXPANDTABS | DT_TABSTOP | DT_LEFT | DT_VCENTER | DT_SINGLELINE, &p);
///
///	return p.uiLengthDrawn;
///}

UINT CFont::GetDrawFlags(bool wrap, bool ells, EXAlign xa, EYAlign ya)
{
	return	DT_NOCLIP | DT_NOPREFIX | DT_EXPANDTABS |  DT_TABSTOP |
			(ells ? DT_END_ELLIPSIS | DT_WORD_ELLIPSIS : 0) |
			(wrap ? DT_WORDBREAK : 0) |
			(xa == EXAlign::Left ? DT_LEFT : 0) |
			(xa == EXAlign::Center ? DT_CENTER : 0) |
			(xa == EXAlign::Right ? DT_RIGHT : 0) |
			(ya == EYAlign::Bottom ? DT_BOTTOM : 0) |
			(ya == EYAlign::Center ? DT_VCENTER /*|DT_SINGLELINE*/ : 0) |
			(ya == EYAlign::Top ? DT_TOP : 0);
}