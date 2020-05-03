#pragma once
#include "Int64.h"

namespace uos
{
	enum class EEthType
	{
		Null, integer, string, dynamic, array
	};

	struct CSolidityValue
	{

		EEthType							Type;
		char								One[32]={};
		CArray<CSolidityValue>				Many;

		CSolidityValue & operator = (const CSolidityValue & v)
		{
			Type = v.Type;
			Many = v.Many;
			CopyMemory(One, v.One, sizeof(One));
			return *this;
		}

		CSolidityValue();
		CSolidityValue(boost::multiprecision::uint256_t v);
		CSolidityValue(const CString & v);
		CSolidityValue(EEthType t)
		{
			Type = t;
		}
		CSolidityValue(const CSolidityValue & v)
		{
			Type = v.Type;
			Many = v.Many;
			CopyMemory(One, v.One, sizeof(One));
		}
		CSolidityValue(EEthType t, CSolidityValue & v, int n=1)
		{
			Type = t;

			for(int i=0; i<n; i++)
			{
				Many.push_back(v);
			}
		}


		int GetHeaderSize();
		int GetDataSize();

		CString		Encode();
		void		Decode(wchar_t * p);

		static CString									Encode(char * d, int s);
		static CString									Encode(int v);
		static char										ToHex(wchar_t h, wchar_t l);
		
		uint256											GetInt();
		CString											GetString();
		CArray<uint256>									GetIntArray();
	};
}
