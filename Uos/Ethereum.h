#pragma once
#include "PipeClient.h"
#include "HttpClient.h"
#include "SolidityValue.h"

namespace uos
{
	class CEthereum : public IType
	{
		public:
			CLevel2	*									Level;
			CPipeClient	*								Pipe = null;
			const CString								Contract = L"0xfAFaE0FAd572365b25bBDDc8Ef10593d8F5F4f77";
			const CString								Infura = L"https://mainnet.infura.io/v3/b032fdb13aff414e9f215d53cf0a2953";
			
			CHttpClient									Http;
		
			UOS_RTTI
			CEthereum(CLevel2 * l);
			~CEthereum();

			void										Call(CString const & sig, CArray<CSolidityValue> * in, CArray<CSolidityValue> * out, std::function<void()> done);
			CString										Encode(CArray<CSolidityValue> & values);
			void										Decode(CString const & d, CArray<CSolidityValue> & values);
			CString										EncodeSignature(const CString & s);
			void										IsRegistered(const CString & name);
			
	};
}
