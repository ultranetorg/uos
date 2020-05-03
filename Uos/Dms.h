#pragma once
#include "Ethereum.h"
#include "Fdn.h"

namespace uos
{
	class CProductRelease
	{
		public:
			CString		Product;
			CVersion	Version;
			CString		Cid;
	};

	class CDms : virtual public IType
	{
		public:
			CLevel2 *									Level;
			CEthereum *									Ethereum;
			CFdn *										Fdn;

			UOS_RTTI
			CDms(CLevel2 * l, CFdn * fdn);
			~CDms();
			
			void										FindReleases(const CString & product, CString const & platfrom, std::function<void(CArray<uint256> &)> ok);
			void										GetRelease(CString const & product, uint256 id, std::function<void(CString, CVersion, CString)> ok);
	};
}