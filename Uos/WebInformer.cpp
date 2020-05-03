#include "StdAfx.h"
#include "WebInformer.h"

using namespace uos;
//using namespace boost::posix_time;

CWebInformer::CWebInformer(CLevel2 * l, CServer * s, CString & dir)
{
	Level = l;
	Thread = null;

	Config = new CConfig(Level, s->MapPath(L"WebInformer.xon"), CPath::Join(dir, L"WebInformer.xon"));
		
	RootParameter = Config->Root;

	Level->Nexus->Initialized += ThisHandler(OnNexusInitialized);
}
	
CWebInformer::~CWebInformer()
{
	Config->Save();
	delete Config;
}

void CWebInformer::OnNexusInitialized()
{
	QueryCheck();
}

void CWebInformer::AddInfo(const CString & name, const CString & value)
{
	OutData[name] = value;
}

void CWebInformer::QueryCheck()
{
	OutData.clear();
	QueryGatheringStarted();
		
	auto now = CDateTime::UtcNow();
	auto lastcheck = RootParameter->GetOr<CDateTime>(L"LastCheck", CDateTime::Min);

	if(now - lastcheck  > 60*60*24)
	{
		Url = CUrl(UO_WEB_SERVICE_ACTIVITY);
		Url[L"Version"]	= L"3";
		Url[L"Product"]	  = Level->Core->Product.Name;
		Url[L"ProductVersion"] = Level->Core->Product.Version.ToString();
		Url[L"ProductStage"] = Level->Core->Product.Stage;
		Url[L"BuildConfiguration"] = Level->Core->Product.Build;
		Url[L"OS"] = Level->Core->Product.Platform;
		Url[L"OSVersion"] = Level->Core->Os->GetVersion().ToStringERB();
		Url[L"UNID"] = Level->Core->Unid;
														
		for(auto & i : OutData)
		{
			Url[i.first] = i.second;
		}

		auto r = new CHttpRequest(Level, Url.ToString());
		r->Recieved =	[this, r]
						{
							try
							{
								auto & d = CTonDocument(CXonTextReader(&r->Stream));

								auto p = d.One(L"Update/Product");

								if(p)
								{
									Data.Version		= CVersion(p->One(L"Version")->Get<CString>());
									Data.DownloadPageUrl= p->One(L"DownloadPageUrl")->Get<CString>();

									RootParameter->One(L"LatestProductVersion")->Set(Data.Version.ToString());
									RootParameter->One(L"DownloadPageUrl")->Set(Data.DownloadPageUrl);
									RootParameter->One(L"LastCheck")->Set(CDateTime::UtcNow());

									ProductInfoRetrieved(&Data);
								
									Level->Log->ReportMessage(this, L"Communication successful: LatestProductVersion=%s", Data.Version.ToString());
								}

							}
							catch(CException &)
							{
								Level->Log->ReportWarning(this, L"Communication failure");
							}

							delete r;
						};
		r->Failed = [r]
					{
						delete r;
					};
		r->Send();
	}
	else
	{
		Data.Version = CVersion(RootParameter->Get<CString>(L"LatestProductVersion"));
		Data.DownloadPageUrl = RootParameter->Get<CString>(L"DownloadPageUrl");
		ProductInfoRetrieved(&Data);
	}
}

