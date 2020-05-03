#include "StdAfx.h"
#include "Int64.h"

using namespace uos;

const std::wstring CInt64::TypeName = L"int64";

std::wstring CInt64::GetTypeName()
{
	return TypeName;
}

void CInt64::Read(CStream * s)
{
	s->Read(&Value, 8);
}

int64_t CInt64::Write(CStream * s)  
{
	return s->Write(&Value, 8);
}

void CInt64::Write(std::wstring & s)
{
	s += ToString(Value);
}

void CInt64::Read(const std::wstring & b)
{
	Value = CInt64::Parse(b);
}

ISerializable * CInt64::Clone()
{
	return new CInt64(Value);
}

bool CInt64::Equals(const ISerializable & a) const
{
	return Value == ((CInt64 &)a).Value;
}

CString CInt64::ToString(int64_t i)
{
	std::wostringstream s;
	s << i;
	return s.str();
}

int64_t CInt64::Parse(const std::wstring & s)
{
	return _wtoi64(s.data());
}

int64_t CInt64::Parse(const std::wstring & s, const std::wstring & f)
{
	int64_t v;
	swscanf_s(s.data(), f.data(), &v);
	return v;
}

bool CInt64::Valid(const CString & s)
{
	return !s.empty() && s.find_first_not_of(L"0123456789", s[0]==L'+' || s[0]==L'-' ? 1 : 0) == std::string::npos;
}
