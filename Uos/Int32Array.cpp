#include "StdAfx.h"
#include "Int32Array.h"

using namespace uos;

const std::wstring CInt32Array::TypeName = L"int32.array";

std::wstring CInt32Array::GetTypeName()
{
	return TypeName;
}

void CInt32Array::Read(CStream * s)
{
	auto n = ReadSize(s);
	resize((size_t)n);
	s->Read(data(), n * 4);
}

int64_t CInt32Array::Write(CStream * s)  
{
	auto c = WriteSize(s, size());
	return c + s->Write(data(), size() * 4);
}

void CInt32Array::Write(std::wstring & s)
{
	wchar_t b[32];

	for(auto i : *this)
	{
		StringCbPrintf(b, sizeof(b), L"%d", i);

		s += b;
		if(&i != &back())
			s += L" ";
	}
}

void CInt32Array::Read(const std::wstring & v)
{
	*this = CConverter::ParseArray<int>(v);
}

ISerializable * CInt32Array::Clone()
{
	return new CInt32Array(*this);
}

bool CInt32Array::Equals(const ISerializable & a) const
{
	return (size() == ((CInt32Array &)a).size()) && memcmp(&front(), &((CInt32Array &)a).front(), size() * sizeof(int)) == 0;
}

