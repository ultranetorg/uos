#pragma once
#include "IArranger.h"

namespace uos
{
	class CSortKey
	{
		public:
			typedef boost::variant<int, float, CString> CValueType;
			CArray<CValueType>							Values;
			IArrangable *								Arrangable;

			void										AddValue(const CString & v);
			void										RemoveValue(const CString & v);
			
			bool										operator == (const CSortKey & a);
			
			CSortKey(IArrangable * a);
			CSortKey(IArrangable * a, float v);
			~CSortKey(){}
	};
	
	bool operator < (const CSortKey & a, const CSortKey & b);
	
}
