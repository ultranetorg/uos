#pragma once
#include "Element.h"

namespace uos
{
	class UOS_WORLD_LINKING CScrollbar : public CElement
	{
		public:
			CStyle *									Style;
			CEngine *									Engine;
			
			float										Value = 0;
			float										Total = NAN;
			float										Visible = NAN;

			float										T;
			float										P;
			float										G;
			float										S;

			CFloat3										Point;

			CEvent<>										Scrolled;

			UOS_RTTI
			CScrollbar(CWorldLevel * l, CStyle * s, CString const & name = CGuid::Generate64(GetClassName()));
			~CScrollbar();

			void SetTotal(float v);
			void SetVisible(float v);
			void										ValueToPosition();
			virtual void								Draw();
			void										OnMoveInput(CActive * r, CActive * s, CMouseArgs * a);

			virtual void								UpdateLayout(CLimits const & l, bool apply) override;
	};
}
