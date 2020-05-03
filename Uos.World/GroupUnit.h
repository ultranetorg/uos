#pragma once
#include "Unit.h"

namespace uos
{
	class UOS_WORLD_LINKING CGroupUnit : public CUnit
	{
		public:
			CSize										Size;

			UOS_RTTI
			CGroupUnit(CWorldServer * l, CString & dir, CUol & entity, CString const & type,	CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			CGroupUnit(CWorldServer * l, CString & dir, CString const & name,					CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			~CGroupUnit();

			using CUnit::Save;
			using CUnit::Load;
			
			void										Open() override;
			void										Close() override;

			virtual CHeaderTab *						AddModel(CUol & m, CModel * model);
			virtual void								OpenModel(CUol & e);
			virtual void								CloseModel(CUol & m);

			CSpaceBinding<CVisualSpace> &				AllocateVisualSpace(CViewport * vp) override;
 			CSpaceBinding<CActiveSpace> &				AllocateActiveSpace(CViewport * vp) override;
 			void 										DeallocateVisualSpace(CVisualSpace * s) override;
 			void 										DeallocateActiveSpace(CActiveSpace * s) override;
 
			CSize										DetermineSize(CSize & smax, CSize & s) override;
			CTransformation								DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t) override;
			EPreferedPlacement 							GetPreferedPlacement() override;
			CCamera *									GetPreferedCamera() override;

			CSize										Measure() override;
			void 										Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin) override;
			void 										Update() override;
			void 										Normalize() override;
			void 										Interact(bool e) override;

			bool 										ContainsEntity(CUol & o) override;
			bool 										ContainsAvatar(CUol & o) override;

			CString &									GetDefaultInteractiveMasterTag() override;

	};
}

