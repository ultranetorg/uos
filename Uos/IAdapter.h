#pragma once
#include "Transformation.h"

namespace uos
{
	class IArranger;

	class IAdapter
	{
		public:
//			virtual CTransformation						GetTransformation()=0;
//			virtual void								SetNormalPosition(float d, float h, float yaw)=0;
			//virtual IArranger *							GetArranger()=0;
			virtual CTransformation						GetNormalTransformation()=0;
			virtual bool								IsManual()=0;
			virtual bool								IsReal()=0;

			virtual ~IAdapter(){}
	};
}