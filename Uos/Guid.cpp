#include "stdafx.h"
#include "Guid.h"
#include "Base58.h"

using namespace uos;

CGuid::CGuid(unsigned int bit) : CArray<unsigned char>(bit / 8)
{
}

CGuid CGuid::New64()
{
	CGuid o(64);
	GUID g;
	CoCreateGuid(&g);

	unsigned char * p = (unsigned char *)&g;

	for(int i = 0; i < o.size(); i++)
	{
		o[i] = *p;
		p++;
	}

	return o;
}

CGuid CGuid::New128()
{
	CGuid o(128);
	GUID g;
	CoCreateGuid(&g);

	auto p = (unsigned char *)&g;

	for(int i = 0; i < o.size(); i++)
	{
		o[i] = *p;
		p++;
	}

	return o;
}

CGuid CGuid::New256()
{
	CGuid o(256);

	GUID g[2];
	CoCreateGuid(&g[0]);
	CoCreateGuid(&g[1]);

	auto p = (unsigned char *)g;
	
	for(auto i = 0u; i < o.size(); i++)
	{
		o[i] = *p;
		p++;
	}

	return o;
}

CString CGuid::Generate64()
{
	//GUID g;
	//CoCreateGuid(&g);
	//
	//return CString::Format(L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X", g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);

	return CBase58::Encode(New64());
}

CString CGuid::Generate64(const CString & pre)
{
	//return pref + L"-" + CConverter::Base64Encode((const unsigned char *)&g, sizeof(GUID)).substr(0, 22);
	return pre + L"-" + CBase58::Encode(New64());
}
