#pragma once
#include "Storage.h"

namespace uos
{
	class CFdn : public IType
	{
		public:
			CLevel2 *									Level;
			ipfs::Client *								Ipfs;

			CMap<CArray<char> *, std::function<void(CArray<char> *)>>		Loads;

			UOS_RTTI
			CFdn(CLevel2 * l);
			~CFdn();
			
			void										Load(const CString & addr, std::function<void(CArray<char> *)> & done);
	};
}

