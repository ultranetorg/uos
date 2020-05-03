#pragma once

namespace uos
{
	class IFileService
	{
		public:
			virtual void 		WriteUTF16(const CString & path, const CString & text) = 0;
			virtual void 		WriteANSI(const CString & path, const CAnsiString & text) = 0;
			
			virtual ~IFileService(){}
	};
}