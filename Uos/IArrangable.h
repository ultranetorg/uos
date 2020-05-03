#pragma once
#include "IAdapter.h"
#include "Size.h"
#include "Animation.h"

namespace uos
{
	class IArranger;

	class IArrangable
	{
		public:
			virtual CSize & 							GetArea()=0;
			virtual void								SetAdapter(IArranger * ar, IAdapter * ad)=0;
			virtual IAdapter *							GetAdapter(IArranger * ar)=0;
			virtual void								Arrange(CTransformation & t, CAnimation & a)=0;
		
			virtual ~IArrangable(){};
	};
}