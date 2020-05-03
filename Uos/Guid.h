#pragma once
#include "String.h"

namespace uos
{
	class UOS_LINKING CGuid : public CArray<unsigned char>
	{
		public:
			CGuid(unsigned int bit);

			static CGuid								New64();			
			static CGuid								New128();			
			static CGuid								New256();			
			static CString								Generate64(const CString & pref);			
			static CString								Generate64();			
	};
}
