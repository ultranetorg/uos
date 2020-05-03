#include "StdAfx.h"
#include "Float.h"

using namespace uos;

const std::wstring CFloat::TypeName = L"float32";
const float	CFloat::PI = float(M_PI);

std::wstring CFloat::GetTypeName()
{
	return TypeName;
}

void CFloat::Read(CStream * s)
{
	s->Read(&Value, 4);
}

int64_t CFloat::Write(CStream * s)  
{
	return s->Write(&Value, 4);
}

void CFloat::Write(std::wstring & s)
{
	wchar_t b[325+1];
	StringCbPrintf(b, sizeof(b), L"%g", Value);
	s += b;
}

void CFloat::Read(const std::wstring & b)
{
	Value = Parse(b);
}

ISerializable * CFloat::Clone()
{
	return new CFloat(Value);
}

bool CFloat::Equals(const ISerializable & a) const
{
	return Value == ((CFloat &)a).Value;
}

float CFloat::Parse(const CString & s)
{
	return (float)_wtof(s.c_str());
}

CString CFloat::NiceFormat(float x)
{
	if(!std::isfinite(x))
	{
		return L"      nan";
	}

	if(x == 0.f)
	{
		return L"        0";
	}

	if(x < 0 && fabs(x) < 1e-6)
	{
		return L"       -0";
	}

	if(x > 0 && fabs(x) < 1e-6)
	{
		return L"       +0";
	}

	wchar_t s[16];

	if(fabs(x) < 1)
		StringCchPrintf(s, _countof(s), L"%9.6f", x);
	else if(1e-6 < fabs(x) && fabs(x) < 1e6)
		StringCchPrintf(s, _countof(s), L"%9.3f", x);
	else
		StringCchPrintf(s, _countof(s), L"%9.2e", x);

	return s;
}
