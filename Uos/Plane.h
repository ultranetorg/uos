#pragma once
#include "Float4.h"

namespace uos
{
	class UOS_LINKING CPlane
	{
		public:
			float				A;
			float				B;
			float				C;
			float				D;
			CFloat3			Origin;
			CFloat3			Normal;

			bool										IsFrontFacingTo(CFloat3& direction);
			float										GetSignedDistanceTo(CFloat3& point);
			CFloat3										Intersect(CRay & r);
			float										GetAngle(CFloat3 & v);
			
			CPlane();
			CPlane(float nx, float ny, float nz);
			CPlane(CFloat3& origin, CFloat3& normal);
			CPlane(CFloat3& p1, CFloat3& p2, CFloat3& p3);
	};
}