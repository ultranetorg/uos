#pragma once
#include "License.h"

namespace uos
{
	class ILicenseService
	{
		public:
//			virtual bool								IsLicenseValid(CString & name, CVersion & v)=0;
			virtual void								SetLicense(const CString & kp, const CString & data, int dt, int dl)=0;
			virtual void								AddLicensableItem(const CString & name, CVersion & v)=0;
			
			virtual void								InstallLicense(char * key, const CString & filename)=0;

			virtual CString								GetStatus()=0;
			virtual CString								GetDescription()=0;
			virtual CString 							GetKeyPath()=0;
			virtual CString 							GetType()=0;
			
			virtual bool								IsAllowed()=0;
			virtual bool								IsValid()=0;

			virtual ~ILicenseService(){}

	};
}