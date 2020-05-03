#pragma once
#include "IArrangable.h"

namespace uos
{
	enum EArrangement
	{
		EArrangement_No = 0,
		EArrangement_Default,
		EArrangement_SortingV,
		EArrangement_SortingH,
		EArrangement_SortingR
	};

	class IArranger
	{
		public:
			//virtual void								Add(IArrangable * wn)=0;
			//virtual void								Remove(IArrangable * wn)=0;

			//virtual void								ProcessNodeAxisEvent(IArrangable * wn, CInteractiveAxisEventArgs & a)=0;
			//virtual void								ProcessNodeMouseButtonEvent(IArrangable * wn, CInteractiveEventArgs & a)=0;
			
			virtual ~IArranger(){}
	};
}