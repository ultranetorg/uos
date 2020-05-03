#pragma once
#include "IMenu.h"
#include "World.h"

namespace uos
{
	class UOS_WORLD_LINKING CCard : public CRectangle
	{
		public:
			CWorld *									Level;
			ECardTitleMode							TitleMode = ECardTitleMode::Null;
			CElement *									Face = null;
			CText *										Text = null;
			CString										Title;
			float										Spacing = 5.f;
			CAvatarMetrics								Metrics;
			CFloat4										TextColor;
			CString										Type;

			UOS_RTTI
			CCard(CWorld * l, const CString & name = GetClassName());
			~CCard();

			void										SetFace(CElement * f);
			void										SetTitleMode(ECardTitleMode tp);
	};
}