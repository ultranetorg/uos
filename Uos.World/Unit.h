#pragma once
#include "Area.h"
#include "Header.h"

namespace uos
{
	class CWorldServer;

	class UOS_WORLD_LINKING CUnit : public CArea
	{

		public:
			CWorldServer *								Level;

			CObject<CWorldEntity>						Entity;

			CString										LastInteractiveMaster;
			CString										LastNoninteractiveMaster;
			CString										LastMaster;

			CVisualGraph *								VisualGraph = null;
			CActiveGraph *								ActiveGraph = null;

			CHeader *									Header = null;
			CView *										HeaderView = null;

			UOS_RTTI
			CUnit(CWorldServer * l, CView * hview, CVisualGraph * vg, CActiveGraph * ag, CString const & name);
			~CUnit();
			
			using CArea::Save;
			using CArea::Load;
			
			virtual void								Open(){}
			virtual void								Close(){}

			CSpaceBinding<CVisualSpace> &				AllocateVisualSpace(CViewport * vp) override;
			CSpaceBinding<CActiveSpace> &				AllocateActiveSpace(CViewport * vp) override;
			void 										DeallocateVisualSpace(CVisualSpace * s) override;
			void 										DeallocateActiveSpace(CActiveSpace * s) override;

			void										Save(CXon * x) override;
			void										Load(CXon * x) override;

			virtual CSize 								DetermineSize(CSize & smax, CSize & s)=0;
			virtual CTransformation						DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t)=0;
			virtual EPreferedPlacement 					GetPreferedPlacement()=0;
			virtual CCamera *							GetPreferedCamera()=0;

			virtual void								Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin)=0;
			virtual void								Update()=0;

			virtual void 								Normalize()=0;

			virtual void								Interact(bool e)=0;

			virtual bool								ContainsEntity(CUol & o)=0;
			virtual bool								ContainsAvatar(CUol & o)=0;

			virtual CString &							GetDefaultInteractiveMasterTag()=0;
	};
}