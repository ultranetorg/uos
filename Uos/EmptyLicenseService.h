#pragma once
#include "ILicenseService.h"
#include "License.h"
#include "IType.h"

namespace uos
{
	class UOS_LINKING CEmptyLicenseService : public ILicenseService, public IType
	{
		public:
//			bool										IsLicenseValid(CString & name, CVersion & v);
			void										SetLicense(const CString & kp, const CString & data, int dt, int dl);
			void										AddLicensableItem(const CString & name, CVersion & v);
			
			void										InstallLicense(char * key, const CString & filename);

			CString										GetStatus();
			CString										GetDescription();
			CString 									GetKeyPath();
			CString 									GetType();
			
			bool										IsAllowed();
			bool										IsValid();

			UOS_RTTI
			CEmptyLicenseService();
			~CEmptyLicenseService();

		private:
	};
}