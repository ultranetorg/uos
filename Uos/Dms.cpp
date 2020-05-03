#include "stdafx.h"
#include "Dms.h"

using namespace uos;

CDms::CDms(CLevel2 * l, CFdn * fdn)
{
	Level = l;
	Fdn = fdn;
	Ethereum = new CEthereum(l);
}

CDms::~CDms()
{
	delete Ethereum;
}

void CDms::FindReleases(const CString & product, CString const & platfrom, std::function<void(CArray<uint256> &)> ok)
{
	CArray<CSolidityValue> in;
	in.push_back(CSolidityValue(product));
	in.push_back(CSolidityValue(platfrom));

	auto out = new CArray<CSolidityValue>();
	out->push_back(CSolidityValue(EEthType::dynamic, CSolidityValue(EEthType::integer)));

	Ethereum->Call(L"FindBuilds(string,string)", &in, out,	[ok, out]
															{
																ok((*out)[0].GetIntArray());
																delete out;
															}) ;
}

void CDms::GetRelease(CString const & product, uint256 id, std::function<void(CString, CVersion, CString)> ok)
{
	CArray<CSolidityValue> in;
	in.push_back(CSolidityValue(product));
	in.push_back(CSolidityValue(id));
	
	auto out = new CArray<CSolidityValue>();
	out->push_back(CSolidityValue(EEthType::string));
	out->push_back(CSolidityValue(EEthType::array,  CSolidityValue(EEthType::integer), 4));
	out->push_back(CSolidityValue(EEthType::string));

	Ethereum->Call(L"GetBuild(string,uint256)", &in, out,	[ok, out]
															{
																auto v = (*out)[1].GetIntArray();
																ok((*out)[0].GetString(), CVersion(uint(v[0]), uint(v[1]), uint(v[2]), uint(v[3])), (*out)[2].GetString());
																delete out;
															});
}