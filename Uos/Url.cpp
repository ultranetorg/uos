#include "StdAfx.h"
#include "Url.h"

using namespace uos;

const CUrl &		CUrl::Empty = CUrl();
const std::wstring	CUrl::TypeName = L"url";

CUrl::CUrl(const CString & addr)
{
	Read(addr);
}

CUrl::CUrl(const CUrl & l, const CString & n, const CString & v) : CUrl(l)
{
	Query[n] = v;
}

CUrl::CUrl(const CString & protocol, const CString & path)
{
	Protocol = protocol;
	Path = path;
}

CString & CUrl::operator[](const CString & p)
{
	return Query[p];
}

bool CUrl::PathEquals(CUrl & u)
{
	return	Protocol == u.Protocol && Domain == u.Domain && Path == u.Path;
}

bool CUrl::IsEmpty()
{
	return Protocol.empty() && Domain.empty() && Path.empty() && Query.empty();
}

CString CUrl::ToString() const
{
	CString e;

	if(!Protocol.empty())
	{
		e += Protocol + L"://";
	}

	e += Domain;

	if(!Path.empty())
	{
		e += L"/" + Path;
	}

	if(!Query.empty())
	{
		e += L"?" + CString::Join(Query, [this](auto i){ return i.first + L"=" + EncodeParameter(i.second); }, L"&");
	}
	return e;
}

CString CUrl::GetParamOrEmpty(const CString & p)
{
	auto & i = Query.find(p);
	return i != Query.end() ? i->second : CString();
}

bool CUrl::operator<(const CUrl & a) const
{
	if(Protocol != a.Protocol)
		return Protocol < a.Protocol;
	else if(Domain != a.Domain)
		return Domain < a.Domain;
	else if(Path != a.Path)
		return Path < a.Path;
	else
		return Query < a.Query;
}

CString CUrl::EncodeParameter(const CString &value)
{
	auto v = value;
	v.SelfReplace(L"&", L"%26");
	//v.SelfReplace(L"=", L"%3D");

	return v;
/*
	std::wostringstream escaped;
	escaped.fill(L'0');
	escaped << std::hex;

	for(auto i = value.begin(), n = value.end(); i != n; ++i)
	{
		CString::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if(iswalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << std::uppercase;
		escaped << L'%' << std::setfill(L'0') << std::setw(4) << int(c);
		escaped << std::nouppercase;
	}

	return escaped.str();*/
}

CString CUrl::DecodeParameter(const CString & src)
{
	auto v = src;
	v.SelfReplace(L"%26", L"&");

	return v;
/*
	CString dst;
	dst.reserve(src.size());

	CString::const_iterator j;
	for(auto i : src)
	{
		if(*i == L'%')
		{
			j = i;
			if(++i == src.end()) break;
			if(++i == src.end()) break;
			if(++i == src.end()) break;
			if(++i == src.end()) break;

			wchar_t a = *(++j);
			wchar_t b = *(++j);
			wchar_t c = *(++j);
			wchar_t d = *(++j);
			if(iswxdigit(a) && iswxdigit(b))
			{
				if(a >= L'a')	a -= L'a' - L'A';
				if(a >= L'A')	a -= (L'A' - 10);
				else			a -= L'0';

				if(b >= L'b')	b -= L'b' - L'A';
				if(b >= L'A')	b -= (L'A' - 10);
				else			b -= L'0';

				if(c >= L'c')	c -= L'c' - L'A';
				if(c >= L'A')	c -= (L'A' - 10);
				else			c -= L'0';

				if(d >= L'd')	d -= L'd' - L'A';
				if(d >= L'A')	d -= (L'A' - 10);
				else			d -= L'0';

				dst += (a << 12) + (b << 8) + (c << 4) + d;
			}
		}
		else if(*i == L'+')
			dst += L' ';
		else
			dst += *i;
	}

	return dst;*/
}
/*

CString CUrl::ToName()
{
	if(Domain.EndsWith(L"ultranet"))
	{
		return Path;
	}
	else
	{
		return Domain + L"@" + Path.Split(L"/")[0];
	}
}
*/

bool CUrl::operator!=(const CUrl & a)
{
	if(Protocol != a.Protocol || Path != a.Path)
	{
		return true;
	}

	auto & ap = a.Query.begin();
	auto & bp = Query.begin();

	while(ap != a.Query.end() && bp != Query.end())
	{
		if(ap->second != bp->second)
		{
			return true;
		}

		ap++;
		bp++;
	}
	return false;
}

bool CUrl::operator==(const CUrl & a) const
{
	if(Protocol != a.Protocol || Path != a.Path)
	{
		return false;
	}

	auto & ap = a.Query.begin();
	auto & bp = Query.begin();

	while(ap != a.Query.end() && bp != Query.end())
	{
		if(ap->second != bp->second)
		{
			return false;
		}

		ap++;
		bp++;
	}
	return true;
}

// ISerializable

std::wstring CUrl::GetTypeName()
{
	return TypeName;
}

void CUrl::Read(CStream * s)
{
	CString t;
	t.Read(s);
	Read(t);
}

int64_t CUrl::Write(CStream * s)  
{
	return ToString().Write(s);
}

void CUrl::Write(std::wstring & s)
{
	s += ToString();
}

void CUrl::Read(const std::wstring & addr)
{
	Read(addr, &Protocol, &Domain, &Path, &Query);
}

ISerializable * CUrl::Clone()
{
	return new CUrl(*this);
}

bool CUrl::Equals(const ISerializable & a) const
{
	return CUrl::operator==( static_cast<const CUrl&>(a) );
}

void CUrl::Read(const std::wstring & addr, CString * pr, CString * domain, CString * path, CMap<CString, CString> * query)
{
	wchar_t * c = const_cast<wchar_t *>(addr.data());

	int stage = 0;
	// 1 - protocol is got
	// 2 - domain
	// 3 - path
	// 4 - query
	// 5 - anchor
	auto a = c;

	CString name;

	do
	{
		if(*c == L':' && stage == 0)
		{
			if(pr)
				pr->assign(a, c);

			c++;
			if(*c == L'/') c++;
			if(*c == L'/') c++;
			a = c;
			stage = 1;
		}
		else if((*c == L'/' || *c == L'?' || *c == L'#' || *c == 0) && stage <= 1)
		{
			if(domain)
				domain->assign(a, c);

			stage = 2;

			if(*c != 0)
			{
				if(*c == '?')
				{
					stage = 3;
				}
				if(*c == '#')
				{
					stage = 4;
				}
				a = ++c;
			}

			if(*c == 0)
				a = null;
		}
		else if((*c == L'?' || *c == L'#' || *c == 0) && stage <= 2)
		{
			if(path)
				path->assign(a, c);

			stage = 3;

			if(*c != 0)
			{
				if(*c == '?')
				{
					stage = 3;
				}
				if(*c == '#')
				{
					stage = 4;
				}
				a = ++c;
			}

			if(*c == 0)
				a = null;
		}
		else if((*c == L'#' || *c == 0) && stage <= 3)
		{
			if(query)
			{
				auto p = a;

				while(p != c && *p != L'#')
				{
					while(p != c && *p != L'=' && *p != L'&' && *p != L'#') 
						p++;
	
					name.assign(a, p);
	
					if(*p == L'=')
					{
						a = ++p;
		
						while(p != c && *p != L'&' && *p != L'#')
							p++;
		
						(*query)[name].assign(DecodeParameter(CString(a, p)));
					}

					if(p != c)
						p++;
						
					a = p;
				}
			}

			stage = 4;

			if(*c != 0)
				a = ++c;
			else
				a = null;
		}
		else
			c++;

	} while(a);
}

CString CUrl::Join(CString const & a, CString const & b)
{
	auto & aa = a.EndsWith(L"/") ? a.substr(0, a.size()-1) : a;
	auto & bb = b.StartsWith(L"/") ? b.substr(1, b.size()-1) : b;

	CString r = L"";

	if(!aa.empty() && !bb.empty())
	{
		r = aa + L"/" + bb;
	}
	else if(!aa.empty() && bb.empty())
	{
		r = a;
	}
	else if(aa.empty() && !bb.empty())
	{
		r = b;
	}
	else if(a + b == L"/")
		return L"/";

	if(r.EndsWith(L"/"))
	{
		r.resize(r.size() - 1);
	}

	return r;
}
