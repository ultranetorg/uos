#pragma once
#include "ILicenseService.h"
#include "License.h"
#include "NativePath.h"
#include "FileService.h"

namespace uos
{
	class UOS_LINKING CLicenseService : public ILicenseService, public IType
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
			CLicenseService(const CString & folderAppDataMightywillAximionLicense);
			~CLicenseService();

		private:
			CString										LicenseFolderPath;

			CString										KeyPath;
			CString										Licensee;
			CString										Type;
			CString										Date;
			int											TrialDaysTotal;
			int											TrialDaysLeft;

			CList<CLicenseItem>							LicensableItems;
			CList<CLicenseItem>							LicensedItems;
	};
}