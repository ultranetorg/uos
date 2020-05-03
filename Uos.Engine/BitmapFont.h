#pragma once
#include "EngineLevel.h"

namespace uos
{
/*
	#define D3DFONT_BOLD        0x0001
	#define D3DFONT_ITALIC      0x0002
	#define D3DFONT_ZENABLE     0x0004

	#define D3DFONT_CENTERED_X  0x0001
	#define D3DFONT_CENTERED_Y  0x0002
	#define D3DFONT_TWOSIDED    0x0004
	*/
	struct CCharCoord
	{
		float L, T, R, B;
	};

	struct CLineMeasurement
	{
		int		Count = 0;
		float	Width = 0.f;
	};

	class UOS_ENGINE_LINKING CBitmapFont
	{
		public:
			float										Scale;
			float										Height;
			int											TextureWidth;
			int											TextureHeight;
			 
			TEXTMETRIC									TextMetric;	
			int	*										CharWidths;
			CCharCoord *								TexCoords;

			CEngineLevel *								Level; 

			HFONT										HFont;
			HFONT										HFontOld;

			CString										Family;
			float										Size;
			bool										IsBold;
			bool										IsItalic;
			
//			int											Flags;

			HDC											FontDC;
			HBITMAP										FontBitmap;
			HGDIOBJ										FontBitmapOld;
			int *										FontBitmapBits;
			BITMAPINFO									FontBitmapInfo;

			float										GetHeight();
			CFloat2										GetTextExtent(const CString & t);
			CFloat2										GetTextExtent(const CString & t, int wlimit);
			int											GetTextLenghtForSpecifiedWidth(const CString & t, int n);
			CFloat2										GetTextureCharSize(wchar_t c);
			
			CFloat2 Measure(const CString & t, float wmax, float hmax, bool wrap);
						
			CBitmapFont(CEngineLevel * l, const CString & name, float size, bool bold, bool italic);
			~CBitmapFont();

		private:
		
			HFONT										CreateGDIFont(HDC dc, float scale);
			bool										PaintAlphabet(HDC dc, GLYPHSET * gs, BOOL bMeasureOnly);

			void										Init();
			void ResizeFontBitmap(int w, int h);
			void										Free();
	};
}