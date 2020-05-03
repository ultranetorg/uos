#pragma once
#include "Unit.h"

namespace uos
{
	class UOS_WORLD_LINKING CSingleUnit : public CUnit
	{
		public:
			CObject<CModel>								Model;

			UOS_RTTI
			CSingleUnit(CWorldServer * l, CModel * m,											CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			CSingleUnit(CWorldServer * l, CString & gpath, CUol & entity, CString const & type,	CView * hview, CVisualGraph * vg, CActiveGraph * ag);
			CSingleUnit(CWorldServer * l, CString & gpath,										CView * hview, CVisualGraph * vg, CActiveGraph * ag, CString const & name);
			~CSingleUnit();

			using CArea::Save;
			using CArea::Load;

			void										Initialize();
			
 			CSpaceBinding<CVisualSpace> &				AllocateVisualSpace(CViewport * vp) override;
 			CSpaceBinding<CActiveSpace> &				AllocateActiveSpace(CViewport * vp) override;
 			void 										DeallocateVisualSpace(CVisualSpace * s) override;
 			void 										DeallocateActiveSpace(CActiveSpace * s) override;
 
			CSize										DetermineSize(CSize & smax, CSize & s) override;
			CTransformation								DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t) override;
			EPreferedPlacement 							GetPreferedPlacement() override;
			CCamera 	*								GetPreferedCamera() override;

 			CSize										Measure() override;

			void 										Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin) override;
			void 										Update() override;

			void 										Normalize() override;

			void 										Interact(bool e) override;

			bool 										ContainsEntity(CUol & o) override;
			bool 										ContainsAvatar(CUol & o) override;

			CString &									GetDefaultInteractiveMasterTag() override;

			void										Save(CXon * x) override;
			void										Load(CXon * x) override;

			void Open() override;


			void Close() override;

	};
}