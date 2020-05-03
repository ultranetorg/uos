#pragma once
#include "Text.h"
#include "Group.h"
#include "Stack.h"
#include "Area.h"

namespace uos
{
	class CWorldServer;
	class CAvatarCard;

	class CHeaderTab
	{
		public:
			CObject<CModel>								Model;
			CAvatarCard *										Card;
			CWorldServer *								Level;
			bool										Shown = false;
			CSize										Size = CSize::Nan;
			bool										Selected = false;

			CHeaderTab(CWorldServer * l, CUol & m, CModel * model);
			~CHeaderTab();
	};

	class UOS_WORLD_LINKING CHeader : public CArea
	{
		public:
			CList<CHeaderTab *>							Tabs;
			CVisualGraph *								VisualGraph = null;
			CActiveGraph *								ActiveGraph = null;
			CStack *									Stack = null;
			CEvent<CUol &>								Selected;
			ECardTitleMode							TitleMode = ECardTitleMode::Null;
			CSize										IconSize = {16, 16, 0};
			CSize										Size = CSize::Nan;

			UOS_RTTI
			CHeader(CWorldServer * l, CVisualGraph * vg, CActiveGraph * ag, CView * view);
			~CHeader();
			
			void										Select(CUol & m);
			CHeaderTab *								AddTab(CUol & m, CModel * model);
			CSpaceBinding<CVisualSpace> &				AllocateVisualSpace(CViewport * vp) override;
			CSpaceBinding<CActiveSpace> &				AllocateActiveSpace(CViewport * vp) override;
			void										DeallocateVisualSpace(CVisualSpace * s) override;
			void										DeallocateActiveSpace(CActiveSpace * s) override;
			void										Update(CSize & s);
	};
}