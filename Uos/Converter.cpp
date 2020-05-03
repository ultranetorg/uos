#include "StdAfx.h"
#include "Converter.h"

using namespace uos;

CString CConverter::ToString(size_t i)
{
	std::wostringstream s;
	s << i;
	return s.str();
}

CString CConverter::ToString(GUID guid)
{
	wchar_t szGuidW[40] = {0};
	StringFromGUID2(guid, szGuidW, 40);
	return szGuidW;
}

GUID CConverter::ToGUID(const CString & t)
{
	GUID g;
	IIDFromString(t.c_str(), &g);
	return g;
}

DWORD CConverter::ToARGB(const CString & name)
{
	auto v = ParseArray<float>(name);
	if(v.size() == 3)
	{
		return 0xFF << 24 | ((UCHAR)(v[0]*255))<<16 | ((UCHAR)(v[1]*255))<<8 | ((UCHAR)(v[2]*255));
	}
	if(v.size() == 4)
	{
		return  ((UCHAR)(v[3]*255))<<24 | 
				((UCHAR)(v[0]*255))<<16 | 
				((UCHAR)(v[1]*255))<<8 | 
				((UCHAR)(v[2]*255));
	}
	return 0xFFFFFFFF;
}

