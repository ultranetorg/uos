#pragma once
#include "GroupUnit.h"

namespace uos
{
	class UOS_WORLD_LINKING CHighspaceGroupUnit : public CGroupUnit
	{
		public:
			CSize										Size;

			UOS_RTTI
			CHighspaceGroupUnit(CWorldServer * l, CString & dir, CUol & entity, CString const & type,	CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			CHighspaceGroupUnit(CWorldServer * l, CString & dir, CString const & name,					CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			~CHighspaceGroupUnit();

			using CGroupUnit::Save;
			using CGroupUnit::Load;

			void 										Initialize();

			void										Save(CXon * x) override;
			void										Load(CXon * x) override;

	};
}