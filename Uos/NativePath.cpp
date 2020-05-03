#include "StdAfx.h"
#include "NativePath.h"

using namespace uos;


CNativePath::CNativePath()
{
}

CNativePath::~CNativePath()
{
}

bool CNativePath::IsUNCServer(const CString & a)
{
	return PathIsUNCServer(a.c_str()) == TRUE;
}

bool CNativePath::IsUNCServerShare (const CString & a)
{
	return PathIsUNCServerShare(a.c_str()) == TRUE;
}

bool CNativePath::IsRoot(const CString & a)
{
	return PathIsRoot(a.c_str()) != FALSE;
}

bool CNativePath::IsDirectory(const CString & a)
{
	return PathIsDirectory(a.c_str()) != FALSE;
}

bool CNativePath::IsFile(const CString & a)
{
	return PathFileExists(a.c_str()) && PathIsDirectory(a.c_str()) == FALSE;
}

CString CNativePath::Join(const CString & a, const CString & b)
{
	//wchar_t p[32768];
	//PathCombine(p, a.c_str(), b.c_str());
	//return p;
	auto & aa = a.EndsWith(L"\\") ? a.substr(0, a.size()-1) : a;
	auto & bb = b.StartsWith(L"\\") ? b.substr(1, b.size()-1) : b;

	if(bb == L".")
	{ 
		bb = L"";
	}

	CString r = L"";

	if(!aa.empty() && !bb.empty())
	{
		r = aa + L"\\" + bb;
	}
	else if(!aa.empty() && bb.empty())
	{
		r = a;
	}
	else if(aa.empty() && !bb.empty())
	{
		r = b;
	}
	else if(a + b == L"\\")
		return L"\\";

	if(r.EndsWith(L"\\"))
	{
		r.resize(r.size() - 1);
	}

	return r;
}

CString CNativePath::Join(const CString & a, const CString & b, const CString & c)
{
	wchar_t ab[32768];
	PathCombine(ab, a.c_str(), b.c_str());

	wchar_t abc[32768];
	PathCombine(abc, ab, c.c_str());
	return abc;
}

CString CNativePath::GetDirectoryPath(const CString & a)
{
	wchar_t b[32768];

	wcscpy_s(b, _countof(b), a.data());
	PathRemoveFileSpec(b);
	return b;
}

CString CNativePath::GetFileName(const CString & a)
{
	return PathFindFileName(a.c_str());
}

CString CNativePath::GetFileNameBase(const CString & a)
{
	wchar_t * p = (wchar_t *)_alloca((a.size() + 1) * sizeof(wchar_t));
	
	wcscpy_s(p, a.size() + 1, a.data());

	auto fname = PathFindFileName(p);
	PathRemoveExtension(fname);

	return fname;
}

CString CNativePath::Canonicalize(const CString & a)
{
	wchar_t p[32768];
	PathCanonicalize(p, a.c_str());
	return p;
}

CString CNativePath::GetDirectoryName(const CString & a)
{
	return PathFindFileName(a.c_str());
}

CString CNativePath::AddBackslash(const CString & a)
{
	wchar_t b[32768];
	wcscpy_s(b, _countof(b), a.c_str());
	PathAddBackslash(b);
	return b;
}

CString CNativePath::GetFileExtension(const CString & a)
{
	return PathFindExtension(a.c_str());
}

CString CNativePath::ReplaceFileName(const CString & path, const CString & filename)
{
	return Join(GetDirectoryPath(path), filename);
}

bool CNativePath::IsRelative(const CString & s)
{
	return PathIsRelative(s.c_str()) != 0;
}

CString CNativePath::GetSafe(const CString & s)
{
	CString out = s;
	for(auto & i : out)
	{
		if(i == L'<' || i == L'>' || i == L':' || i == L'"' || i == L'/' || i == L'\\' || i == L'|' || i == L'?' || i == L'*')
		{
			i = L'_';
		}
	}
	return out;
}	

CString CNativePath::FindOnPath(const CString & file)
{
	wchar_t path[MAX_PATH];
	wcscpy_s(path, _countof(path), file.c_str());
	PathFindOnPath(path, null);
	return path;
}

CString CNativePath::RemoveBackslash(const CString & file)
{
	wchar_t path[MAX_PATH];
	wcscpy_s(path, _countof(path), file.c_str());
	PathRemoveBackslash(path);
	return path;
}

CString CNativePath::EscapeRegex(CString &regex)
{
	CString r = regex;

	r.SelfReplace(L"\\", L"\\\\");
	r.SelfReplace(L"^",	 L"\\^");
	r.SelfReplace(L".",	 L"\\.");
	r.SelfReplace(L"$",	 L"\\$");
	r.SelfReplace(L"|",	 L"\\|");
	r.SelfReplace(L"(",	 L"\\(");
	r.SelfReplace(L")",	 L"\\)");
	r.SelfReplace(L"[",	 L"\\[");
	r.SelfReplace(L"]",	 L"\\]");
	r.SelfReplace(L"*",	 L"\\*");
	r.SelfReplace(L"+",	 L"\\+");
	r.SelfReplace(L"?",	 L"\\?");
	r.SelfReplace(L"/",	 L"\\/");

	return r;
}

bool CNativePath::MatchWildcards(const CString &text, CString wildcardPattern, bool caseSensitive /*= false*/)
{
	// Escape all regex special chars
	EscapeRegex(wildcardPattern);

	// Convert chars '*?' back to their regex equivalents
	boost::replace_all(wildcardPattern, L"\\?", L".");
	boost::replace_all(wildcardPattern, L"\\*", L".*");

	std::wregex pattern(wildcardPattern, caseSensitive ? std::wregex::basic : std::wregex::icase);

	return regex_match(text, pattern);
}

CString CNativePath::GetTmp()
{
	wchar_t path[MAX_PATH];
	GetTempPath(_countof(path), path);
	return path;
}
