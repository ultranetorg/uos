#pragma once
#include "Triangle.h"

namespace uos
{
	class UOS_LINKING CPolygon
	{
		public:
			static bool									Real(CArray<CFloat2> & v);
			static bool									Contains(CArray<CFloat2> & v, CFloat2 & p);
			CFloat2										MinXVertex(CArray<CFloat2> & v);
			CFloat2										MaxXVertex(CArray<CFloat2> & v);
			CFloat2										MinYVertex(CArray<CFloat2> & v);
			CFloat2										MaxYVertex(CArray<CFloat2> & v);
	};
}

