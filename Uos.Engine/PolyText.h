#pragma once

#if 0

#include "BitmapFont.h"
#include "Rectangle.h"
#include "Style.h"

namespace mw
{
	enum class EVerticalAlignment
	{
		Top, Bottom, Center
	};
		
	enum class ETextAlign
	{
		Horizontal = 1, Vertical = 2
	};

	class UOS_WORLD_CLASS CText : public CElement
	{
		public:
			CWorldLevel *								Level;
			CGod *										Engine;
			//CVisual *									VText;
			CMesh *										VMesh;
			//CMesh *										AMesh;
			CFont *										Font;
			//EVerticalAlignment							VAlign = EVerticalAlignment::Center;
			CString										Text;
			int											Color;
			bool										Wrap;

			bool										NeedApply = true;

			UOS_DEFINE_RTTI
			//CText(CWorldLevel * l);
			CText(CWorldLevel * l, const CString & v, int c, bool wrap, CFont * f);
			CText(CWorldLevel * l, const CString & v, int c = 0xFFFF'FFFF, bool wrap = true, const CString & family = L"MS Sans Serif", int size = 9, bool bold = false, bool italic = false);
			~CText();

			void										SetFont(CFont * f);
			void										SetText(const CString & t);
			void										SetWrap(bool e);
			void										SetColor(int c);
			//void										SetVerticalAlignment(EVerticalAlignment a);


			void										Apply();

			virtual void								SetArea(CArea & a) override;

			void										UpdateArea(const CArea & max, bool apply);
			CArea										UpdateContentLayout(CArea & imax, bool apply);

	};
}

#endif