#pragma once
#include "IOs.h"
#include "IType.h"
//#include "Exception.h"

//#include "Log.h"

namespace uos
{

	class UOS_LINKING COsNT : public IOs, public IType
	{
		public:
			virtual	void								SaveDCToFile(LPCTSTR FileName, HDC hsourcedc, int Width, int Height);

			virtual CVersion							GetVersion();
			virtual bool								Is64();
	
			CList<CString>								OpenFileDialog(DWORD options, CArray<std::pair<CString,CString>> & types = CArray<std::pair<CString,CString>>());

			CString										ComputerName();
			CString										GetUserName();

			void										RegisterUrlProtocol(const CString & pwProtocolName, const CString & pwCompanyName, const CString & pwAppPath);

			BOOL										SetPrivilege(HANDLE hToken, /* access token handle */ LPCTSTR lpszPrivilege, /* name of privilege to enable/disable */ BOOL bEnablePrivilege /* to enable or disable privilege */);
			
			UOS_RTTI
			COsNT();
			~COsNT();

		protected:
			//CLog *										Log;

			CString										WindowClass;
			CVersion		 							Version;


	};
}
