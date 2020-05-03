#include "StdAfx.h"
#include "Version.h"

using namespace uos;

CVersion::CVersion()
{
	Era			= 0;
	Release		= 0;
	Build		= 0;
	Revision	= 0;
}

CVersion::CVersion(const CString & v)
{
	std::wistringstream s(v);
	
	s>>Era;
	s.ignore(v.size(), L'.');
	s>>Release;
	s.ignore(v.size(), L'.');
	s>>Build;
	s.ignore(v.size(), L'.');
	s>>Revision;
}

CVersion::CVersion(uint e, uint r, uint b, uint rv)
{
	Era			= e;
	Release		= r;
	Build		= b;
	Revision	= rv;
}

CVersion::~CVersion(void)
{
}

CString CVersion::ToString()
{
	return CString::Format(L"%d.%d.%d.%d", Era, Release, Build, Revision);
}

CString CVersion::ToStringERB()
{
	return CString::Format(L"%d.%d.%d", Era, Release, Build);
}

CVersion CVersion::FromModule(HMODULE m)
{
	CVersion v;

	wchar_t s[4096];
	GetModuleFileName(m, s, 4096);
	int size = ::GetFileVersionInfoSize(s, null);
	if(size > 0)
	{
		void * p = malloc(size);
		::GetFileVersionInfo(s, 0, size, p);
		VS_FIXEDFILEINFO * vi;
		UINT l;
		VerQueryValue(p, L"\\", (void **)&vi, &l);

		v.Era		= HIWORD(vi->dwFileVersionMS);
		v.Release	= LOWORD(vi->dwFileVersionMS);
		v.Build		= HIWORD(vi->dwFileVersionLS);
		v.Revision	= LOWORD(vi->dwFileVersionLS);

		free(p);
	}
	return v;
}

CVersion CVersion::FromFile(const CString & path)
{
	CVersion v;

	int size = ::GetFileVersionInfoSize(path.c_str(), null);
	if(size > 0)
	{
		void * p = malloc(size);
		::GetFileVersionInfo(path.c_str(), 0, size, p);
		VS_FIXEDFILEINFO * vi;
		UINT l;
		VerQueryValue(p, L"\\", (void **)&vi, &l);

		v.Era		= HIWORD(vi->dwFileVersionMS);
		v.Release	= LOWORD(vi->dwFileVersionMS);
		v.Build		= HIWORD(vi->dwFileVersionLS);
		v.Revision	= LOWORD(vi->dwFileVersionLS);

		free(p);
	}
	return v;
}

CString CVersion::GetStringInfo(const CString & path, const CString & v)
{
	BOOL rc;
	DWORD *pdwTranslation;
	UINT nLength;
	CString out;

	int size = ::GetFileVersionInfoSize(path.c_str(), null);
	if(size > 0)
	{
		void * p = alloca(size);
		::GetFileVersionInfo(path.c_str(), 0, size, p);


		rc = ::VerQueryValue(p, L"\\VarFileInfo\\Translation", (void**) &pdwTranslation, &nLength);
		if (!rc)
			return L"";

		TCHAR szKey[2000];
		wsprintf(szKey, L"\\StringFileInfo\\%04x%04x\\%s", LOWORD (*pdwTranslation), HIWORD (*pdwTranslation),	v.c_str());	


		UINT l;
		wchar_t * b;
		VerQueryValue(p, szKey, (void **)&b, &l); 
		out = b;

		//free(p);
	}
	return out;
}	

bool CVersion::operator==(CVersion & v)
{
	return Era==v.Era && Release==v.Release && Build==v.Build && Revision==v.Revision;
}

bool CVersion::operator!=(CVersion & v)
{
	return !(*this == v);
}


bool CVersion::operator > (CVersion & v)
{
	if(Era > v.Era) return true;
	if(Era < v.Era) return false;

	if(Release > v.Release)	return true;
	if(Release < v.Release)	return false;

	if(Build > v.Build)	return true;
	if(Build < v.Build)	return false;

	if(Revision > v.Revision)	return true;
	if(Revision < v.Revision)	return false;

	return false;
}

bool CVersion::operator < (CVersion & v)
{
	if(Era < v.Era) return true;
	if(Era > v.Era) return false;

	if(Release < v.Release)	return true;
	if(Release > v.Release)	return false;

	if(Build < v.Build)	return true;
	if(Build > v.Build)	return false;

	if(Revision < v.Revision)	return true;
	if(Revision > v.Revision)	return false;

	return false;
}

bool CVersion::IsGreaterER(const CVersion & v)
{
	if(Era > v.Era) return true;
	if(Era < v.Era) return false;

	if(Release > v.Release)	return true;
	if(Release < v.Release)	return false;

	return false;
}

bool CVersion::IsGreaterOrEqER(const CVersion & v)
{
	if(Era > v.Era) return true;
	if(Era < v.Era) return false;

	if(Release > v.Release)	return true;
	if(Release < v.Release)	return false;

	return true;
}

