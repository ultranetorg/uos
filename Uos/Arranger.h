#pragma once
#include "IArranger.h"
#include "SortKey.h"

namespace uos
{
	class CArranger
	{
		public:
			void										FitHorizontally(CArray<CSortKey> & ads, float total, float spacing, int start, int & end, float & maxy);
			float										FindMaxHeight(CArray<CSortKey> & ads, int start, int end);
			float										FindMaxWidth(CArray<CSortKey> & ads, int start, int end);
			float										GetNext(float c, float * a, float * b, float v, float spacing);
			//bool static								SortByKeyByObbYFunctor(IArrangable * l, IArrangable * r);
			//bool static								SortByKeyByObbXFunctor(IArrangable * l, IArrangable * r);

			CArranger();
			~CArranger();
	};
}