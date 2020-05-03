#include "stdafx.h"
#include "SerializableBuffer.h"

using namespace uos;

const std::wstring CSerializableBuffer::TypeName = L"binary";

std::wstring CSerializableBuffer::GetTypeName()
{
	return TypeName;
}

void CSerializableBuffer::Read(CStream * s)
{
	int size;
	s->Read(&size, 4);
	*this = s->Read(size);
}

int64_t CSerializableBuffer::Write(CStream * s)  
{
	int64_t size = GetSize();
	s->Write(&size, 8);
	return 8 + s->Write(GetData(), size);
}

void CSerializableBuffer::Write(std::wstring & s)
{
/*
	std::wstring o;
	o.reserve(GetSize()*2);

	unsigned char * p = (unsigned char *)GetData();
	wchar_t d[3];
	for(int i=0; i<GetSize(); i++)
	{
		_itow_s(*p++, d, _countof(d), 16);
		o += d;
	}

	return o;*/

	s += CBase64::Encode((unsigned char *)GetData(), GetSize());
}

void CSerializableBuffer::Read(const std::wstring & v)
{
	/*Set(null, v.size()/2);

	auto s = v.data();
	unsigned char * d = (unsigned char *)GetData();
	wchar_t * e;
	wchar_t t[3];
	t[2] = 0;

	for(auto i=0U; i<v.size()/2; i++)
	{
		t[0] = *s++;
		t[1] = *s++;
		*d = (unsigned char)wcstol(t, &e, 16);
		if(t == e)
		{
			throw CException(HERE, L"Invalid hex number");
		}
		d++;
	}*/
	*this = CBase64::Decode(v);
}

ISerializable * CSerializableBuffer::Clone()
{
	return new CSerializableBuffer(*this);
}

bool CSerializableBuffer::Equals(const ISerializable & a) const
{
	auto & aa = (CSerializableBuffer &)a;
	auto & bb = (CSerializableBuffer &)*this;

	return (bb.GetSize() == aa.GetSize()) && memcmp(bb.GetData(), aa.GetData(), (size_t)bb.GetSize()) == 0;
}

