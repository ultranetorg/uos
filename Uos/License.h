#pragma once
#include "Converter.h"
#include "Version.h"

namespace uos
{
	struct CProductInfo
	{
		CString			HumanName;
		CString			Name;
		CString			Description;
		CString			Stage;
		CString			Build;
		CString			CompanyName;
		CString			Copyright;
		CString			WebPageHome;
		CString			WebPageSupport;
		CVersion		Version;
		CString			Platform;
		CString			Namespace;

		CString ToString(CString const & a)
		{	
			CString o;
			for(auto i : a)
			{
				if(!o.empty())
					o += L" ";

				if(i == L'N')
					o += HumanName;
				if(i == L'I')
					o += Name;
				if(i == L'V')
					o += Version.ToStringERB();
				if(i == L'S')
					o += Stage;
				if(i == L'P')
					o += Platform;
				if(i == L'B')
					o += Build;
			}
			return o;
		}

	};

	class UOS_LINKING CLicenseItem
	{
		public:
//			bool										IsValid(CString & name, CVersion & v);
//			bool										IsRegistered();

			CString										Name;
			CVersion									Version;

			CLicenseItem(const CString & n, CVersion & v);

		private:
	};
}
	
