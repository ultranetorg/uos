#include "StdAfx.h"
#include "EmptyLicenseService.h"

using namespace uos;

CEmptyLicenseService::CEmptyLicenseService()
{
}

CEmptyLicenseService::~CEmptyLicenseService()
{
}

void CEmptyLicenseService::AddLicensableItem(const CString & name, CVersion & v)
{
}

void CEmptyLicenseService::SetLicense(const CString & kp, const CString & data, int dt, int dl)
{
}

bool CEmptyLicenseService::IsAllowed()
{
	return true;
}

bool CEmptyLicenseService::IsValid()
{
	return true;
}

CString CEmptyLicenseService::GetStatus()
{
	return L"Eternal license";
}

CString CEmptyLicenseService::GetDescription()
{
	return L"Eternal license";
}

CString CEmptyLicenseService::GetKeyPath()
{
	return L"";
}

CString CEmptyLicenseService::GetType()
{
	return L"";
}

void CEmptyLicenseService::InstallLicense(char * key, const CString & filename)
{
}

