#include "StdAfx.h"
#include "Int32.h"

using namespace uos;

const std::wstring CInt32::TypeName = L"int32";

std::wstring CInt32::GetTypeName()
{
	return TypeName;
}

void CInt32::Read(CStream * s)
{
	s->Read(&Value, 4);
}

int64_t CInt32::Write(CStream * s)  
{
	return s->Write(&Value, 4);
}

void CInt32::Write(std::wstring & s)
{
	s += ToString(Value);
}

void CInt32::Read(const std::wstring & b)
{
	Value = CInt32::Parse(b);
}

ISerializable * CInt32::Clone()
{
	return new CInt32(Value);
}

bool CInt32::Equals(const ISerializable & a) const
{
	return Value == ((CInt32 &)a).Value;
}

CString CInt32::ToString(int i)
{
	std::wostringstream s;
	s << i;
	return s.str();
}

int CInt32::Parse(const std::wstring & s)
{
	return _wtoi(s.data());
}

bool CInt32::Valid(const CString & s)
{
	return !s.empty() && s.find_first_not_of(L"0123456789", s[0]==L'+' || s[0] == L'-' ? 1 : 0) == std::string::npos;
}
