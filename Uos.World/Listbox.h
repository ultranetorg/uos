#pragma once
#include "Rectangle.h"
#include "Layout.h"

namespace uos
{
	class UOS_WORLD_LINKING CListbox : public CRectangle
	{
		public:
			using CElement::UpdateLayout;

			CList<CElement *>							Items;
			EYAlign										YAlign = EYAlign::Top;
			CAnimation									Animation;
			CFloat2										Scroll;

			UOS_RTTI
			CListbox(CWorldLevel * l, CStyle * s, CString const & name = GetClassName());
			~CListbox();

			void										AddItem(CElement * e, CList<CElement *>::iterator before);
			void										AddBack(CElement * e);
			void										AddFront(CElement * e);
			void										RemoveItem(CElement * e);

			void										UpdateLayout(CLimits const & l, bool apply) override;
			void										Arrange(CAnimation & a);

			void										OnMouse(CActive *, CActive *, CMouseArgs * a);
	};
}
