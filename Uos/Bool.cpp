#include "StdAfx.h"
#include "Bool.h"

using namespace uos;

const std::wstring CBool::TypeName = L"bool";

std::wstring CBool::GetTypeName()
{
	return TypeName;
}

void CBool::Read(CStream * s)
{
	s->Read(&Value, 1);
}

int64_t CBool::Write(CStream * s)  
{
	return s->Write(&Value, sizeof(Value));
}

void CBool::Write(std::wstring & s)
{
	s += Value ? L"y" : L"n";
}

void CBool::Read(const std::wstring & b)
{
	const wchar_t * v = b.c_str();

	if(lstrcmpi(v, L"true") == 0 || lstrcmpi(v, L"yes") == 0 || lstrcmpi(v, L"y") == 0)
	{
		Value = true;
	}
	else if(lstrcmpi(v, L"false") == 0 || lstrcmpi(v, L"no") == 0 || lstrcmpi(v, L"n") == 0)
	{
		Value = false;
	}
	else
		throw CException(HERE, L"Incorrect field");
}

ISerializable * CBool::Clone()
{
	return new CBool(Value);
}

bool CBool::Equals(const ISerializable & a) const
{
	return Value == ((CBool &)a).Value;
}

