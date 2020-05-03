#pragma once
#include "String.h"

namespace uos
{
	class IType
	{
		public:
			virtual CString &							GetInstanceName()=0;
			template<class T> T	*						As(){ return dynamic_cast<T *>(this); }

			//virtual CString		ToString(){ return L""; }

		protected:
			virtual ~IType(){}
	};
}