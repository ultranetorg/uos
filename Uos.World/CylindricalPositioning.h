#pragma once
#include "Positioning.h"
#include "Avatar.h"

namespace uos
{
	class UOS_WORLD_LINKING CCylindricalPositioning : public CPositioning
	{
		public:
			CWorldLevel *								Level;
			float										H;
			float										R;

			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceRandom;
			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceCenter;
			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceExact;

			CMap<CViewport *, std::function<CMatrix(CViewport *)>>	Matrixes;

			UOS_RTTI
			CCylindricalPositioning(CWorldLevel * l, float h, float r);
			
			CFloat3										GetDirection(CViewport * vp, CActive * node, CActiveSpace * s, CSize & size, CTransformation & t);
			CFloat3										GetPoint(CRay & ray);

			CMatrix										GetMatrix(CViewport * vp) override;
			CTransformation								Move(CPositioningCapture & c, CPick & pick) override;
			CPositioningCapture							Capture(CPick & pick, CSize & size, CMatrix & wm) override;
			CPositioningCapture							Capture(CPick & pick, CSize & size, CFloat3 & offset) override;
			CFloat3										Project(CViewport * vp, CActiveSpace * s, CFloat3 & p) override;
			CFloat3										GetPoint(CViewport * vp, CFloat2 & vpp) override;

			//virtual CTransformation Resize(CPositioningCapture & c, CViewport * vp, CFloat2 & vpp) override;
	};
}