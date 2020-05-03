#pragma once
#include "Rectangle.h"
#include "Text.h"

namespace uos
{
	class UOS_WORLD_LINKING CButton : public CRectangle
	{
		public:
			CText *										Text;
			CEvent<CButton *>							Pressed;
			CStyle *									Style;

			UOS_RTTI
			CButton(CWorldLevel * l, CStyle * s, CString const & name = GetClassName());
			~CButton();

			void										SetText(CString const & t);

			void										OnMouse(CActive *, CActive *, CMouseArgs * a);

			void										UpdateLayout(CLimits const & l, bool apply) override;


			void virtual								LoadProperties(CStyle * s, CXon * n) override;
	};
}
