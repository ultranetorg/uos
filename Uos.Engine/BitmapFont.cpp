#include "stdafx.h"
#include "BitmapFont.h"

using namespace uos;

CBitmapFont::CBitmapFont(CEngineLevel * l, const CString & name, float size, bool bold, bool italic)
{
	Level		= l;

	CharWidths	= null;
	TexCoords	= null;

	Family		= name;
	Size		= size;
	IsBold		= bold;
	IsItalic	= italic;
		
	Init();
}

CBitmapFont::~CBitmapFont()
{
	SelectObject(FontDC, HFontOld);
	DeleteObject(HFont);

	DeleteDC(FontDC);
	
	free(CharWidths);
	//Material->Free();
	//Texture->Free();
}

void CBitmapFont::Free()
{
	if(CharWidths != null && TexCoords != null)
	{
		
		//free(TexCoords);
	}
}

void CBitmapFont::Init()
{
	FontDC				= CreateCompatibleDC(null);
	FontBitmap			= null;
	FontBitmapOld		= null;
	//FontBitmapWidth		= 128;
	//FontBitmapHeight	= 128;
	SetMapMode(FontDC, MM_TEXT);
	SetTextColor(FontDC, RGB(255,255,255));
	SetTextAlign(FontDC, TA_TOP);
	SetBkColor(FontDC, 0x00000000);
	SetBkMode(FontDC, TRANSPARENT);

	Scale = 1.0f; 

	HFont = CreateGDIFont(FontDC, Scale);
	if(HFont != null)
	{
		HFontOld			= (HFONT)SelectObject(FontDC, HFont);
		TextureWidth	= 128;
		TextureHeight	= 128;

		GetTextMetrics(FontDC, &TextMetric);
		int n = TextMetric.tmLastChar+1;			
		Height =  float(TextMetric.tmHeight);

		CharWidths	= (int *)		malloc(n * sizeof(int));
		//TexCoords	= (CCharCoord *)malloc(n*sizeof(CCharCoord));
		//		
		n = GetFontUnicodeRanges(FontDC, null);
		GLYPHSET * gs = (GLYPHSET *)malloc(n);
		gs->cbThis = n;
		GetFontUnicodeRanges(FontDC, gs);
		GetCharWidth32(FontDC, 0, TextMetric.tmLastChar, CharWidths);

		CharWidths[L'\t'] = 8 * TextMetric.tmAveCharWidth;

		//TEXTMETRIC tm;
		//GetTextMetrics(FontDC, &tm);
		//
		//bool e = true;
		//while(!PaintAlphabet(FontDC, gs, true))
		//{
		//	e ? TextureWidth *= 2 : TextureHeight *= 2;
		//	e = !e;
		//}
		//
		//if(TextureWidth > 4096)
		//{
		//	Scale			= 4096 / (float)TextureWidth;
		//	TextureWidth	= 4096;
		//	TextureHeight	= 4096;
		//
		//	bool bFirstRun = true; // Flag clear after first run
		//
		//	do
		//	{
		//		// If we've already tried fitting the new text, the scale is still 
		//		// too large. Reduce and try again.
		//		if( !bFirstRun)
		//			Scale *= 0.9f;
		//
		//		// The font has to be scaled to fit on the maximum texture size; our
		//		// current font is too big and needs to be recreated to scale.
		//		DeleteObject(SelectObject(FontDC, fontOld));
		//		font = CreateGDIFont(FontDC, Scale);
		//		fontOld = (HFONT)SelectObject(FontDC, font);
		//		bFirstRun = false;
		//	} 
		//	while(!PaintAlphabet(FontDC, gs, true));
		//}
		//
		//ResizeFontBitmap(TextureWidth, TextureHeight);
		//
		//if(PaintAlphabet(FontDC, gs, false))
		//{
		//	if(Texture->IsEmpty())
		//	{
		//		Texture->Create(TextureWidth, TextureHeight, 1, ETextureFeature::Dynamic, D3DFMT_A4R4G4B4);
		//	} 
		//	else
		//	{
		//		Texture->Resize(TextureWidth, TextureHeight, false);
		//	}
		//	
		//	WORD* pDst16	= (WORD*)Texture->Lock(); 
		//	int * pSrc32	= FontBitmapBits;
		//
		//	for(int i=0; i<TextureHeight*TextureWidth; i++)
		//	{
		//		if((*pSrc32 & 0x00FFFFFF) != 0)
		//		{
		//			*pDst16 = 0xffff;
		//		}
		//		else
		//		{
		//			*pDst16 = 0x0000;
		//		}
		//		
		//		pSrc32++;
		//		pDst16++;
		//
		//		if((i+1)%TextureWidth == 0)
		//		{
		//			pDst16 += (Texture->GetPitch()/2 - TextureWidth);
		//		}
		//	}
		//	Texture->Unlock();
		//	
		//	if(Level2->Core->IsAdministrating())
		//	{
		//		Texture->SaveToFile(Level2->Core->SuperVisor->GetPathToDataFolder(Family + L".dds"));
		//	}
		//}
		free(gs);
		//SelectObject(FontDC, fontOld);
		//DeleteObject(font);
	}

	//if(FontBitmap != null)
	//{	
	//	SelectObject(FontDC, FontBitmapOld);
	//	DeleteObject(FontBitmap);
	//}
	//DeleteDC(FontDC);

}

void CBitmapFont::ResizeFontBitmap(int w, int h)
{
	if(w > FontBitmapInfo.bmiHeader.biWidth || h > abs(FontBitmapInfo.bmiHeader.biHeight) || FontBitmap == null)
	{	
		if(FontBitmap != null)
		{
			SelectObject(FontDC, FontBitmapOld);
			DeleteObject(FontBitmap);
		}

		ZeroMemory(&FontBitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));
		FontBitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		FontBitmapInfo.bmiHeader.biWidth       =  (int)w;
		FontBitmapInfo.bmiHeader.biHeight      = -(int)h;
		FontBitmapInfo.bmiHeader.biPlanes      = 1;
		FontBitmapInfo.bmiHeader.biCompression = BI_RGB;
		FontBitmapInfo.bmiHeader.biBitCount    = 32;

		FontBitmap = CreateDIBSection(FontDC, &FontBitmapInfo, DIB_RGB_COLORS, (void**)&FontBitmapBits, null, 0);
		FontBitmapOld = SelectObject(FontDC, FontBitmap);
	}
	CGdiRect r(0, 0, w, h);
	FillRect(FontDC, &r.GetAsRECT(), (HBRUSH)GetStockObject(BLACK_BRUSH));
}	

HFONT CBitmapFont::CreateGDIFont(HDC dc, float scale)
{
	INT nHeight    = -MulDiv(int(Size), (INT)(GetDeviceCaps(dc, LOGPIXELSY) * scale), 72);

	DWORD dwBold   = IsBold		? FW_BOLD : FW_NORMAL;
	DWORD dwItalic = IsItalic	? TRUE    : FALSE;

	return CreateFont(nHeight, 0, 0, 0, dwBold, dwItalic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, Family.c_str());
}

bool CBitmapFont::PaintAlphabet(HDC dc, GLYPHSET * gs, BOOL measureOnly)
{
	TCHAR str[2] = L" ";

	int x = 0;
	int y = 0;
	
	ZeroMemory(TexCoords, TextMetric.tmLastChar*sizeof(CCharCoord));

	for(int i=0; i<(int)gs->cRanges; i++)
	{
		for(int j=0; j<gs->ranges[i].cGlyphs; j++)
		{
			wchar_t c = gs->ranges[i].wcLow + j;

			if((x + CharWidths[c]) > TextureWidth)
			{
				x  = 0;
				y += int(Height) + 1;
			}

			if(y + Height > TextureHeight)
				return false;

			if(!measureOnly)
			{
				str[0]=c;
			
				ExtTextOut(dc, x, y, 0, null, str, 1, null);

				TexCoords[c].L = (float)(x					)/TextureWidth;
				TexCoords[c].T = (float)(y					)/TextureHeight;
				TexCoords[c].R = (float)(x + CharWidths[c]	)/TextureWidth;
				TexCoords[c].B = (float)(y + Height			)/TextureHeight;
			}
			x += CharWidths[c];
		}
	}

	return true;
}

CFloat2 CBitmapFont::GetTextExtent(const CString & t)
{
	CFloat2 s;
	float rowWidth  = 0.0f;
	float width     = 0.0f;
	float height    = Height;

	const wchar_t * strText = t.c_str();
	
	while( *strText )
	{
		TCHAR c = *strText++;
		
		if(c > TextMetric.tmLastChar)
		{
			c = TextMetric.tmDefaultChar;
		}

		if(c == L'\n')
		{
			rowWidth = 0.0f;
			height  += Height;
		}

		float tx1 = TexCoords[c].L;
		float tx2 = TexCoords[c].R;

		rowWidth += (tx2-tx1) * TextureWidth / Scale;

		if(rowWidth > width)
			width = rowWidth;
	}

	s.x = width;
	s.y = height;
	
	return s;
}


CFloat2 CBitmapFont::GetTextExtent(const CString & t, int wlimit)
{
	CFloat2 s;
	float wcurrent  = 0.0f;
	float width     = 0.0f;
	float height    = Height;

	const wchar_t * strText = t.c_str();
	
	while( *strText )
	{
		TCHAR c = *strText++;
		
		if(c > TextMetric.tmLastChar)
		{
			c = TextMetric.tmDefaultChar;
		}

		float w = (TexCoords[c].R - TexCoords[c].L) * TextureWidth / Scale;

		if(c == L'\n' || wcurrent + w > wlimit)
		{
			width = max(width, wcurrent);

			wcurrent = 0.0f;
			height  += Height;

			if(c != L'\n')
			{
				wcurrent += w;
			}
		}
		else
			wcurrent += w;
	}

	s.x = width;
	s.y = height;
	
	return s;
}

int CBitmapFont::GetTextLenghtForSpecifiedWidth(const CString & t, int n)
{
	int i = 0;
	float rowWidth  = 0.f;
	float width     = 0.f;
	float height    = Height;

	const wchar_t * strText = t.c_str();
	
	while(strText[i])
	{
		TCHAR c = strText[i];

		if(c > TextMetric.tmLastChar)
		{
			c = TextMetric.tmDefaultChar;
		}

		if(c == L'\n')
		{
			return i;
		}

		float tx1 = TexCoords[c].L;
		float tx2 = TexCoords[c].R;

		rowWidth += (tx2-tx1) * TextureWidth	/ Scale;

		if(rowWidth > width)
			width = rowWidth;
		
		if(rowWidth > n)
		{
			return i;
		}
		i++;
	}
	
	return i;
}

CFloat2 CBitmapFont::GetTextureCharSize(wchar_t c)
{
	FLOAT tl = TexCoords[c].L;
	FLOAT tt = TexCoords[c].T;
	FLOAT tr = TexCoords[c].R;
	FLOAT tb = TexCoords[c].B;

	return CFloat2((tr-tl) * TextureWidth/Scale, (tb-tt) * TextureHeight/Scale);
}

CFloat2 CBitmapFont::Measure(const CString & t, float wmax, float hmax, bool wrap)
{
	DRAWTEXTPARAMS p;
	p.cbSize = sizeof(DRAWTEXTPARAMS);
	p.iLeftMargin = 0;
	p.iRightMargin = 0;
	p.iTabLength = 4;
	p.uiLengthDrawn = 0;

	RECT  r = {0, 0, int(wmax), int(hmax)};
	
	DrawTextEx(FontDC, LPWSTR(t.data()), int(t.size()), &r, DT_CALCRECT | (wrap ? DT_WORDBREAK : 0), &p);
	
	CFloat2 c;
	c.x = float(r.right - r.left);
	c.y = max(r.bottom - r.top, Height);

	return c;
}

float CBitmapFont::GetHeight()
{
	return Height;
}
