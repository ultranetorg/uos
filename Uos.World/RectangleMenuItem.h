#pragma once
#include "Text.h"
#include "IMenu.h"
#include "Rectangle.h"

namespace uos
{
	class UOS_WORLD_LINKING CRectangleMenuItem : public CRectangle, public virtual IMenuItem
	{
		public:
			CStyle *									Style;
			CArea *										Area;

			UOS_RTTI
			CRectangleMenuItem(CWorldLevel * l, CStyle * s, const CString & name = GetClassName());
			~CRectangleMenuItem(){}
	};


	class UOS_WORLD_LINKING CRectangleTextMenuItem : public CRectangleMenuItem
	{
		public:
			CRectangle *								Icon = null;
			CText *										Label;
			CElement *								Arrow = null;

			UOS_RTTI
			CRectangleTextMenuItem(CWorldLevel * l, CStyle * s, CTexture * icon, CString const & text, const CString & name = GetClassName());
			~CRectangleTextMenuItem();

			void										Enable(bool e) override;
			void										Highlight(CArea * a, bool e, CSize & s, CPick * p) override;
	};

	class UOS_WORLD_LINKING CRectangleSeparatorMenuItem : public CRectangleMenuItem
	{
		public:
			UOS_RTTI
			CRectangleSeparatorMenuItem(CWorldLevel * l, CStyle * s);
			~CRectangleSeparatorMenuItem(){}

			virtual void								Highlight(CArea * a, bool e, CSize & s, CPick * p){}
	};
}
