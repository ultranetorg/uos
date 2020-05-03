#pragma once
#include "String.h"

namespace uos
{
	class UOS_LINKING CVersion
	{
		public:
		
			CVersion static								FromModule(HMODULE m);
			CVersion static								FromFile(const CString & path);
			CString	static								GetStringInfo(const CString & path, const CString & v);
		
			CString										ToString();
			CString										ToStringERB();
		
			bool										IsGreaterER(const CVersion &);
			bool										IsGreaterOrEqER(const CVersion &);

			uint										Era;
			uint										Release;
			uint										Build;
			uint										Revision;
			
			bool 										operator == (CVersion &);
			bool 										operator != (CVersion &);
			bool 										operator < (CVersion &);
			bool 										operator > (CVersion &);

			CVersion();
			CVersion(const CString & v);
			CVersion(uint e, uint r, uint b, uint rv);
			~CVersion();
	};
}

