#pragma once
#include "IType.h"
#include "Converter.h"

namespace uos
{
	class UOS_LINKING CFileService : public IType
	{
		public:
			UOS_RTTI
			CFileService();
			~CFileService();
		
			void static									WriteUTF16(const CString & path, const CString & text);
			void static									WriteANSI(const CString & path, const CAnsiString & text);
	};
}