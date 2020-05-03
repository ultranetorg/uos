#pragma once
#include "GroupUnit.h"

namespace uos
{
	class UOS_WORLD_LINKING CLowspaceGroupUnit : public CGroupUnit
	{
		public:
			CSize										Size;

			UOS_RTTI
			CLowspaceGroupUnit(CWorldServer * l, CString & dir, CUol & entity, CString const & type,	CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			CLowspaceGroupUnit(CWorldServer * l, CString & dir, CString const & name,					CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			~CLowspaceGroupUnit();

			using CGroupUnit::Save;
			using CGroupUnit::Load;


			void										Initialize();

			void										Open() override;

			void										OpenModel(CUol & e) override;
 
			CSize										DetermineSize(CSize & smax, CSize & s) override;

			void 										Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin) override;
			void										Interact(bool e) override;
			void 										Update() override;
			void 										Normalize() override;

			void										Save(CXon * x) override;
			void										Load(CXon * x) override;

	};
}