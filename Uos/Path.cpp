#include "StdAfx.h"
#include "Path.h"

using namespace uos;

CString CPath::GetName(CString const & n)
{
	return n.substr(n.find_last_of(L'/') + 1);
}

CString CPath::GetNameBase(CString const & u)
{
	auto n = u;
	auto i = n.find_last_of(L'/');

	if(i != CString::npos)
	{
		n = n.substr(i + 1);
	}

	auto j = n.find_last_of(L'.'); 
	if(j != 0 && j != CString::npos)
	{
		n = n.Substring(0, j);
	}

	return n;
}

CString CPath::Join(CString const & a, CString const & b)
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

CString CPath::ReplaceLast(CString & p, CString const & path)
{
	CString r = p;

	auto f = r.Substring(0, r.find_last_of(L'/'));

	return Join(f, path);
}

CString CPath::ReplaceExtension(CString const & p, CString const & ext)
{
	CString::size_type i = 0;

	CString r = p;

	if((i = r.find_last_of(L".")) != CString::npos)
	{
		r = r.replace(i + 1, r.size() - i - 1, ext);
	}
	else
		r +=  L"." + ext;

	return r;
}

CString CPath::GetDirectoryPath(const CString & addr)
{
	return addr.Substring(0, addr.find_last_of(L'/'));
}

CString CPath::GetExtension(CString & f)
{
	auto i = f.find_last_of(L".");

	if(i != CString::npos)
	{
		return f.Substring(i + 1).ToLower();
	}

	return L"";
}

CString CPath::ReplaceMount(CString & p, CString const & mount)
{
	auto r = p;
	auto i = r.find(L"-");
	auto j = r.find(L"/", i);

	r.erase(i + 1, j - (i+1));
	r.insert(i + 1, mount);

	return r;
}

CString CPath::Universalize(CString const & path)
{
	auto p = path.Replace(L"\\", L"/");
	//p = p.Replace(L":/", L"/");
	return p;
}

CString CPath::Nativize(CString const & path)
{
	auto p = path.Replace(L"/", L"\\");

	//std::wregex pattern(L"^([a-zA-z])\\\\", std::wregex::icase);
	//
	//p = regex_replace(p, pattern, L"$1:\\");
	 
	return p;
}
