#include "StdAfx.h"
#include "SortKey.h"

using namespace uos;

bool uos::operator < (const CSortKey & a, const CSortKey & b)
{
	const CArray<CSortKey::CValueType> & x = a.Values;
	const CArray<CSortKey::CValueType> & y = b.Values;

	for(unsigned int i=0; i<min(x.size(), y.size()); i++)
	{
		if(!(x[i] == y[i]))
		{
			return y[i] < x[i];
		}
	}
	return false;
}

CSortKey::CSortKey(IArrangable * a)
{
	Arrangable = a;
}

CSortKey::CSortKey(IArrangable * a, float v)
{
	Arrangable = a;
	Values.push_back(v);
}

void CSortKey::AddValue(const CString & v)
{
	Values.push_back(CValueType(v));
}

void CSortKey::RemoveValue(const CString & v)
{
	Values.Remove(CValueType(v));
}

bool CSortKey::operator==(const CSortKey & a)
{
	const CArray<CValueType> & x = a.Values;

	for(unsigned int i=0; i<x.size(); i++)
	{
		if(!(x[i] == Values[i]))
		{
			return false;
		}
	}
	return true;
}
