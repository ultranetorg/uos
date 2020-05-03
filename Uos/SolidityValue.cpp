#include "stdafx.h"
#include "SolidityValue.h"

using namespace uos;

CSolidityValue::CSolidityValue()
{
}

CSolidityValue::CSolidityValue(const CString & v)
{
	Type = EEthType::string;

	auto u = v.ToUtf8();

	*((size_t *)One) = v.size();

	Many.resize((u.size() + 31)/32);

	auto c = u.begin();

	for(auto & m : Many)
	{
		m.Type = EEthType::integer;

		for(int i = 31; i>=0 && c != u.end(); i--)
		{
			m.One[i] = *c;
			c++;
		}
	}
}

CSolidityValue::CSolidityValue(uint256 v)
{
	auto p = v.backend().limbs(); 
    auto s = sizeof(*p);

	Type = EEthType::integer;
	CopyMemory(One, p, s * 8);
}

int CSolidityValue::GetHeaderSize()
{
	if(Type == EEthType::integer)	return 32; else
	if(Type == EEthType::string)	return 32; else
	if(Type == EEthType::dynamic)	return 32; else
	if(Type == EEthType::array)		return (int)Many.size() * 32;
	else
		throw CException(HERE, L"Wrong type");
}

int CSolidityValue::GetDataSize()
{
	if(Type == EEthType::integer)
		return 32;
	else if(Type == EEthType::string)
	{
		return 32 + int(Many.size()) * 32;
	}
	else if(Type == EEthType::dynamic || Type == EEthType::array)
	{
		int s = 0;
		for(auto & i : Many)
		{
			s += i.GetDataSize();
		}

		return s;
	}
	else
		throw CException(HERE, L"Wrong type");
}

CString CSolidityValue::Encode(int v)
{
	return Encode((char *)&v, sizeof(v));
}

CString CSolidityValue::Encode()
{
	CString r;

	if(Type == EEthType::integer)
	{
		r += Encode(One, sizeof(One));
	}
	if(Type == EEthType::string)
	{
		r += Encode(One, sizeof(size_t));
		for(auto & i : Many)
		{
			r += i.Encode();
		}
	}
	if(Type == EEthType::dynamic)
	{
		auto s = Many.size();
		r += Encode((char *)&s, sizeof(s));

		for(auto & i : Many)
		{
			r += i.Encode();
		}
	}
	if(Type == EEthType::array)
	{
		for(auto & i : Many)
		{
			r += i.Encode();
		}
	}

	return r;
}

CString CSolidityValue::Encode(char * d, int s)
{
	CString r;

	for(int i = 0; i<s; i++)
	{
		r = CString::Format(L"%02x", (unsigned char)*d) + r;
		d++;
	}

	for(int i=0; i<32-s; i++)
	{
		r = L"00" + r;
	}

	return r;
}

void CSolidityValue::Decode(wchar_t * p)
{
	if(Type == EEthType::integer || Type == EEthType::string || Type == EEthType::dynamic)
	{
		for(auto i=0u; i<64; i+=2)
		{
			One[31 - i/2] = ToHex(p[i], p[i+1]);
		}
	}

	if(Type == EEthType::array)
	{
		for(auto & i : Many)
		{
			i.Decode(p);
			p += i.GetHeaderSize() * 2;
		}
	}

	if(Type == EEthType::string)
	{
		auto c = p + 64;

		auto n = GetInt();
		Many.resize(size_t((n + 31)/32));

		for(auto & m : Many)
		{
			for(int i=31; i>=0 && n>0; i--)
			{
				m.One[i] = ToHex(c[0], c[1]);
				c+=2;
				n--;
			}
		}
	}

	if(Type == EEthType::dynamic)
	{
		auto c = p + 64;

		auto n = (uint64_t)GetInt();
		//Many.resize((n + 31)/32);

		auto v = Many.back(); 
		Many.clear();

		for(uint64_t i=0; i<n; i++)
		{
			Many.push_back(v);
			Many.back().Decode(c);
			c += Many.back().GetHeaderSize() * 2;
		}
	}
}

char CSolidityValue::ToHex(wchar_t h, wchar_t l)
{
	char r = 0;

	if(L'0' <= h && h <= L'9')	r |= (h-L'0') << 4; else 
	if(L'a' <= h && h <= L'z')	r |= (10 + h-L'a') << 4;  else 
	if(L'A' <= h && h <= L'Z')	r |= (10 + h-L'A') << 4;

	if(L'0' <= l && l <= L'9')	r |= (l-L'0'); else
	if(L'a' <= l && l <= L'z')	r |= 10 + (l-L'a'); else
	if(L'A' <= l && l <= L'Z')	r |= 10 + (l-L'A');

	return r;
}

uint256 CSolidityValue::GetInt()
{
	uint256 v;
	v.backend().resize(8, 8);

	auto p = v.backend().limbs(); 
    auto s = sizeof(*p);

	CopyMemory(p, One, s * 8);

	return v;
}

CString CSolidityValue::GetString()
{
	CAnsiString a;

	auto n = GetInt();

	for(auto & m : Many)
	{
		for(int i=31; i>=0 && n > 0; i--)
		{
			a.push_back(m.One[i]);
			n--;
		}
	}

	return CString::FromAnsi(a);
}

CArray<uint256> CSolidityValue::GetIntArray()
{
	CArray<uint256> r;

	for(auto & m : Many)
	{
		r.push_back(m.GetInt());
	}

	return r;
}