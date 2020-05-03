#include "StdAfx.h"
#include "String.h"

using namespace uos;

const std::wstring CString::TypeName = L"char.array";

CString::CString()
{
}

CString::CString(size_t n, wchar_t c)
{
	assign(n, c);
}

CString::CString(const_iterator b, const_iterator e)
{
	assign(b, e);
}

CString::CString(const std::wstring & v)
{
	assign(v);
}

CString::CString(const wchar_t * b, const wchar_t * e)
{
	assign(b, e);
}

CString::CString(const wchar_t * v)
{
	assign(v);
}

CString::CString(const wchar_t * p, int n)
{
	assign(p, n);
}

int CString::Compare(wchar_t const * a, wchar_t const * b)
{
	return wcscmp(a, b);
}

std::wstring CString::GetTypeName()
{
	return TypeName;
}

void CString::Read(const std::wstring & v)
{
	assign(v);
}

void CString::Read(CStream * s)
{
	auto n = ReadSize(s);
	resize((size_t)n);
	s->Read((void *)data(), n * 2);
}

void CString::Write(std::wstring & s)
{
	s += *this;
}

int64_t CString::Write(CStream * s)
{
	auto c = WriteSize(s, size());
	return c + s->Write(data(), size() * 2);
}

bool CString::Equals(const ISerializable & a) const
{
	return *this == static_cast<const CString &>(a);
}

ISerializable * CString::Clone()
{
	return new CString(*this);
}


CArray<CString> CString::Split(const wchar_t * s, bool compress) const
{
	CArray<CString> parts;
	boost::algorithm::split(parts, *this, boost::algorithm::is_any_of(s), compress ? boost::algorithm::token_compress_on : boost::algorithm::token_compress_off);
	return parts;
}

CAnsiString CString::ToAnsi() const
{
	assert(size() < INT_MAX);

	int n = WideCharToMultiByte(CP_ACP, 0, c_str(), (int)size() + 1, null, 0, null, null);
	void * b = alloca(n*sizeof(char));

	WideCharToMultiByte(CP_ACP, 0, c_str(), (int)size()+1, (LPSTR)b, n, null, null);

	return (char *)b;
}

CString CString::FromAnsi(const CAnsiString & source)
{
	return FromAnsi(source.c_str());
}

CString CString::FromAnsi(const char * source)
{
	auto n = strlen(source);

	assert(n < INT_MAX);

	int m = MultiByteToWideChar(CP_ACP, 0, source, int(n), null, 0);

	CString o;
	o.resize(m);

	MultiByteToWideChar(CP_ACP, 0, source, int(n), (LPWSTR)o.data(), int(o.size()));

	return o;
}

CString CString::FromUtf8(CAnsiString const & a)
{
	return FromUtf8(a.data(), int(a.size()));
}

CString CString::FromUtf8(const char * source, int n)
{
	assert(n < INT_MAX);

	auto m = MultiByteToWideChar(CP_UTF8, 0, source, n, NULL, 0);

	CString o;
	o.resize(m);

	MultiByteToWideChar(CP_UTF8, 0, source, n, (LPWSTR)o.data(), int(o.size()));

	return o;
}

CArray<char> CString::ToUtf8() const
{
	assert(size() < INT_MAX);

	CArray<char> o(1024);

	auto n = WideCharToMultiByte(CP_UTF8, 0, data(), (int)size(), o.data(), (int)o.size(), NULL, NULL);

	if(n == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		n = WideCharToMultiByte(CP_UTF8, 0, data(), (int)size(), NULL, 0, NULL, NULL);
		o.resize(n);
		WideCharToMultiByte(CP_UTF8, 0, data(), (int)size(), o.data(), (int)o.size(), NULL, NULL);
	}

	o.resize(n);
	return o;
}

CString & CString::operator=(const CString & s)
{
	assign(s);
	return *this;
}

CString & CString::operator=(ISerializable * s)
{
	assign((CString &)*s);
	return *this;
}

CString CString::Substring(size_t offset /*= 0*/, size_t count /*= INT_MAX*/) const
{
	return substr(offset, count);
}

CString CString::Substring(wchar_t const separ, int index) const
{
	auto i = 0ull;
	auto x = 0ull;
	auto y = find(separ);

	while(x != npos)
	{
		if(i == index)
		{
			return Substring(x, y-x);
		}
		x = y+1;
		y = find(separ, x);
		i++;
	}
	return L"";
}

CString CString::Substring(CString const & separ, int index) const
{
	auto i = 0ull;
	auto x = 0ull;
	auto y = find(separ);

	if(y == npos)
	{
		return index == 0 ? *this : L"";
	}

	while(x != npos)
	{
		if(i == index)
		{
			return Substring(x, y);
		}
		x = y;
		y = find(separ, y);
		i++;
	}
	return L"";
}

CArray<CString> CString::ToLines() const
{
	CArray<CString> lines;

	auto a = begin();
	auto b = begin();
	while(a != end())
	{
		while(b != end() && *b != L'\r'&& *b != L'\n')
		{
			b++;
		}

		lines.push_back(CString(a, b));

		if(b != end())
		{
			if(*b == L'\r')
				b++;

			if(*b == L'\n')
				b++;
		}

		a = b;
	}

	return lines;
}

CString CString::ToUpper()
{
	CString data(size(), L'\0');
	std::transform(begin(), end(), data.begin(), ::toupper);
	return data;
}

CString CString::ToLower()
{
	CString data(size(), L'\0');
	std::transform(begin(), end(), data.begin(), ::tolower);
	return data;
}

bool CString::EqualsInsensitive(const CString & v) const
{
	return _wcsicmp(c_str(), v.c_str()) == 0;
}

bool CString::StartsWith(const CString & v) const
{
	return size() >= v.size() && (compare(0, v.size(), v) == 0);
}

bool CString::EndsWith(const CString & v) const
{
	return size() >= v.size() && (compare(size() - v.size(), v.size(), v) == 0);
}

void CString::SelfReplace(const wchar_t * find_str, const wchar_t * replace_str)
{
	size_type i = 0;

	auto n = wcslen(find_str);
	auto m = wcslen(replace_str);

	while((i = find(find_str, i)) != std::wstring::npos)
	{
		replace(i, n, replace_str);
		i += m;
	}
}

CString CString::ReplaceLast(const CString &find_str, const CString &replace_str) const
{
	size_type i = 0;
	CString out = *this;

	if((i = out.find_last_of(find_str, 0)) != out.npos)
	{
		out.replace(i, find_str.length(), replace_str);
	}
	return out;
}

CString CString::Replace(const CString &find_str, const CString &replace_str) const
{
	size_type i = 0;
	CString out = *this;

	while((i = out.find(find_str, i)) != out.npos)
	{
		out.replace(i, find_str.length(), replace_str);
		i += replace_str.length();
	}
	return out;
}

CList<CString> CString::SplitToList(const wchar_t * s, bool compress /*= false*/) const
{
	CList<CString> parts;
	boost::algorithm::split(parts, *this, boost::algorithm::is_any_of(s), compress ? boost::algorithm::token_compress_on : boost::algorithm::token_compress_off);
	return parts;
}

int CString::Count()
{
	return (int)size();
}

void CString::SetValue(CString const & v)
{
	*this = v;
}

CString uos::CString::GetValue()
{
	return *this;
}

int CString::Compare(IComparable * a)
{
	return wcscmp(data(), ((CString *)a)->data());
}
