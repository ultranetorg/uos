#pragma once
#include "HttpRequest.h"

namespace uos
{
	class CHttpClient
	{
		public:
			CList<CHttpRequest *>						Requests;
			CLevel2 *									Level;

			CHttpClient(CLevel2 * l)
			{
				Level = l;
			}

			~CHttpClient()
			{
			}

			void Send(const CString & url, const CString & method, CList<CString> headers, const CString & content, bool caching, std::function<void(CHttpRequest *)> ok)
			{
				auto r = new CHttpRequest(Level, url);
				r->Method = method;
				r->Headers = headers;
				r->Content = content.ToAnsi();
				r->Caching = caching;
				r->Recieved =	[r, ok]
								{
									ok(r);
									delete r;
								};
				r->Send();
			}
	};
}


