#pragma once
#include "Positioning.h"
#include "Avatar.h"

namespace uos
{
	class UOS_WORLD_LINKING CPolygonalPositioning : public CPositioning
	{
		public:
			CMap<CViewport *, std::function<CTransformation(CViewport *)>>	Transformation;
			CMap<CViewport *, std::function<CArray<CFloat2>()>>				Bounds;

			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceRandom;
			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceCenter;
			std::function<CTransformation(CCamera *, EPreferedPlacement, CPick &, CSize &)>			PlaceExact;

			CPolygonalPositioning();

			CMatrix										GetMatrix(CViewport *) override;
			CFloat3										Project(CViewport * vp, CActiveSpace * s, CFloat3 & p);
			CPositioningCapture							Capture(CPick & pick, CSize & size, CMatrix & wm) override;
			CPositioningCapture							Capture(CPick & pick, CSize & size, CFloat3 & offset);
			CTransformation								Move(CPositioningCapture & c, CPick & pick) override;
			CFloat3										GetPoint(CViewport * vp, CFloat2 & vpp) override;

			CArray<CFloat2>								GetBounds(CViewport * vp);

			//virtual CTransformation Resize(CPositioningCapture & c, CViewport * vp, CFloat2 & vpp) override;
	};
}