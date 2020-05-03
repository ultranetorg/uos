#include "StdAfx.h"
#include "FloatArray.h"

using namespace uos;

const std::wstring CFloatArray::TypeName = L"float32.array";

std::wstring CFloatArray::GetTypeName()
{
	return TypeName;
}

void CFloatArray::Read(CStream * s)
{
	auto n = ReadSize(s);
	resize((size_t)n);
	s->Read(data(), n * 4);
}

int64_t CFloatArray::Write(CStream * s)  
{
	auto c = WriteSize(s, size());
	return c + s->Write(data(), size() * 4);
}

void CFloatArray::Write(std::wstring & s)
{
	wchar_t b[32];

	for(auto i : *this)
	{
		StringCbPrintf(b, sizeof(b), L"%g", i);

		s += b;
		s += L" ";
	}
}

void CFloatArray::Read(const std::wstring & v)
{
	auto & a = CConverter::ParseArray<float>(v);
	assign(a.begin(), a.end());
}

ISerializable * CFloatArray::Clone()
{
	return new CFloatArray(*this);
}

bool CFloatArray::Equals(const ISerializable & a) const
{
	return (size() == ((CFloatArray &)a).size()) && memcmp(&front(), &((CFloatArray &)a).front(), size() * sizeof(int)) == 0;
}
