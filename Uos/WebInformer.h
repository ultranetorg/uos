#pragma once
#include "MemoryStream.h"
#include "Nexus.h"
#include "Config.h"
#include "HttpRequest.h"
#include "XonDocument.h"

//#include "boost/date_time/posix_time/posix_time.hpp"

namespace uos
{
	struct CUpdateData
	{
		CString		Text;
		CVersion	Version;
		CString		DownloadPageUrl;
	};

	class UOS_LINKING CWebInformer : public IType
	{
		public:
			CEvent<>										QueryGatheringStarted;
			CEvent<CUpdateData *>						ProductInfoRetrieved;

			CConfig *									Config;
			CUpdateData									Data;
			CThread *									Thread;
			CXon *										RootParameter;
			
			CUrl										Url;
			CMap<CString, CString>						OutData;
			CLevel2 *									Level;

			UOS_RTTI
			CWebInformer(CLevel2 * l, CServer * s, CString & dir);
			~CWebInformer();

			void										OnNexusInitialized();
			void										AddInfo(const CString & name, const CString & value);
			void										Execute(CUrl & c);
			void										QueryCheck();
			void										GetStatusXMLFromServer(const CString & url, CMemoryStream * s);
	};
}
