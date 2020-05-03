#pragma once

namespace uos
{
	class UOS_LINKING CAngle
	{
		public:
			void static									AdjustRotationToPiRange(float * a);


			float static ToRadian(float a);

	};
}