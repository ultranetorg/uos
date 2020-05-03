#pragma once
#include "Unit.h"
#include "PolygonPositioning.h"
#include "CylindricalPositioning.h"

namespace uos
{
	class CPositioningArea : public CArea
	{
		public:
			CPositioning *								Positioning = null;

			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceNewDefault;
			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceNewExact;
			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceNewConvenient;
			std::function<CTransformation(CPick &, CSize &, CTransformation &)>						PlaceOldDefault;
			

			UOS_RTTI
			CPositioningArea(CWorldServer * l, CString const & name = CGuid::Generate64(GetClassName()));
			~CPositioningArea();

			using CArea::Add;

			void											Open(CArea * a, EAddLocation l, CViewport * , CPick & pick, CTransformation & origin) override;
			void											Close(CArea * a) override;
		
			void											Activate(CArea * a, CViewport * vp, CPick & pick, CTransformation & origin) override;


			void											SetPositioning(CPolygonalPositioning * positioning);
			void											SetPositioning(CCylindricalPositioning * positioning, bool copo);

			CTransformation									PlaceNew(CCamera * c, EPreferedPlacement pp, CPick & pick, CSize & size);
			CTransformation									PlaceOld(CPick & pick, CSize & size, CTransformation & t);
			CPositioningArea *								GetPositioningAncestor();
	};
}
