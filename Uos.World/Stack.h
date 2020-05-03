#pragma once
#include "Rectangle.h"

namespace uos
{
	class CStack : public CRectangle
	{
		public:
			EDirection									Direction = EDirection::X;
			EXAlign										XAlign = EXAlign::Left;
			EYAlign										YAlign = EYAlign::Top;
			CStyle *									Style;
			float										Spacing = 0.f;

			UOS_RTTI
			CStack(CWorldLevel * l, CStyle * s, CString const & name = GetClassName());
			~CStack();

			virtual void								UpdateLayout(CLimits const & l, bool apply) override;
			void										Align();
			virtual void								LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load) override;
			virtual void								LoadProperties(CStyle * s, CXon * n);
	};
}