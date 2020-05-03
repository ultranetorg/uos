#pragma once
#include "IFieldOperations.h"

namespace uos
{
	class UOS_WORLD_LINKING CFieldable
	{
		public:
			IFieldOperations *							Operations = null;
		
			virtual ~CFieldable(){}
			
			virtual void Place(IFieldOperations * s)
			{
				Operations = s;
			}
	};
}

