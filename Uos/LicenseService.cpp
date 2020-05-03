#include "StdAfx.h"
#include "LicenseService.h"
#include "Int32.h"

using namespace uos;

CLicenseService::CLicenseService(const CString & licenseFolderPath)
{
	LicenseFolderPath = licenseFolderPath;
}

CLicenseService::~CLicenseService()
{
}

void CLicenseService::AddLicensableItem(const CString & name, CVersion & v)
{
	LicensableItems.push_back(CLicenseItem(name, v));
}

void CLicenseService::SetLicense(const CString & kp, const CString & data, int dt, int dl)
{
	KeyPath			= kp;
	TrialDaysTotal	= dt;
	TrialDaysLeft	= dl;

	CList<CString> params;
	boost::algorithm::split(params, data, boost::algorithm::is_any_of(L"\r\n"), boost::algorithm::token_compress_on);

	for(auto i : params)
	{
		CList<CString> nameval;
		boost::algorithm::split(nameval, i, boost::algorithm::is_any_of(L"="));

		if(nameval.size() == 2)
		{
			if(nameval.front() == L"Licensee")
			{
				Licensee = nameval.back();
			}
			if(nameval.front() == L"Type")
			{
				Type = nameval.back();
			}
			if(nameval.front() == L"Date")
			{
				Date = nameval.back();
			}
			if(nameval.front() == L"Item")
			{
				CList<CString> sysver;
				boost::algorithm::split(sysver, nameval.back(), boost::algorithm::is_any_of(L";"));
				if(sysver.size() >= 2)
				{
					LicensedItems.push_back(CLicenseItem(sysver.front(), CVersion(sysver.back())));
				}
			}
		}
	}
}

bool CLicenseService::IsAllowed()
{
	if(LicensedItems.empty() && TrialDaysLeft > 0)
	{
		return true;
	}
	else
	{
		return IsValid();
	}
}

bool CLicenseService::IsValid()
{
	int n = 0;
	for(auto li = LicensableItems.begin(); li != LicensableItems.end(); li++)
	{
		for(auto l = LicensedItems.begin(); l != LicensedItems.end(); li++)
		{
			if(li->Name == l->Name && l->Version.IsGreaterOrEqER(li->Version))
			{
				n++;
			}
		}
	}
	return n == LicensableItems.size();
}

CString CLicenseService::GetStatus()
{
	if(!LicensedItems.empty())
	{
		if(IsValid())
		{
			return L"Licensed to: " + Licensee;
		}
		else
		{
			return L"License is outdated. Functionality is limited.";
		}
	}
	if(Type.empty() && TrialDaysLeft > 0)
	{
		return L"Trial license: " + CInt32::ToString(TrialDaysLeft) + L" days left";
	}
	return L"Trial expired. Functionality is limited.";
}

CString CLicenseService::GetDescription()
{
	if(!LicensedItems.empty() && IsValid())
	{
		return  L"Licensee: " + Licensee + L"\r\nType: " + Type + L"\r\nDate: " + Date;
	}
	else
	{
		return GetStatus();
	}
}

CString CLicenseService::GetKeyPath()
{
	return KeyPath;
}

CString CLicenseService::GetType()
{
	return Type;
}

void CLicenseService::InstallLicense(char * key, const CString & filename)
{
	if(!CNativePath::IsDirectory(LicenseFolderPath))
	{
		CreateDirectory(LicenseFolderPath.c_str(), null);
	}
	CFileService::WriteANSI(CNativePath::Join(LicenseFolderPath, filename), key);
}

