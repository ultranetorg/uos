#include "stdafx.h"
#include "HttpRequest.h"

using namespace uos;

CHttpRequest::CHttpRequest(CLevel2 * l, CString const & url)
{
	Level = l;
	Url = url;
}

CHttpRequest::~CHttpRequest()
{
}

void CHttpRequest::Send()
{
	Stream.Clear();
	Level->Core->RunThread(Url,	[this]
							{

								/*
																	hFile = null;

																	HINTERNET hINet;
																	hINet = InternetOpen(Level->Core->Product.ToString(L"NVSPB").c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

																	if(!hINet)
																	{
																		return;
																	}

																	auto t = Headers + L"\r\n\r\n" + Content;
																	hFile = InternetOpenUrl(hINet, Url.c_str(), t.data(), t.size(), (Caching ? 0 : INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_PRAGMA_NOCACHE)|INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_NO_UI, 0);



																	if(hFile)
																	{
																		DWORD dwRead;
																		char buffer[4096];

																		while(InternetReadFile(hFile, buffer, 4096, &dwRead) && !Level->Core->Exiting)
																		{
																			if(dwRead == 0)
																				break;

																			Stream.Write(buffer, dwRead);
																		}

																		//SetEvent(HEvent);

																		InternetCloseHandle(hFile);
																	}
																	else
																	{
																		auto e = GetLastError();

																		wchar_t t[1024];
																		DWORD n = _countof(t);
																		InternetGetLastResponseInfo(&e, t, &n);
																		return;
																	}

																	InternetCloseHandle(hINet);

																	//Stream.Write("\0", 1);

																	Stream.ReadSeek(0);*/
								CURL * curl;

								curl = curl_easy_init();
								if(curl)
								{
									curl_easy_setopt(curl, CURLOPT_URL, Url.ToAnsi().data());
									curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
									curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
							
									curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
									curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

									struct curl_slist *chunk = NULL;

								    if(!Headers.empty())
								    {
										
										for(auto & i : Headers)
										{
											chunk = curl_slist_append(chunk, i.ToAnsi().data());
										}
	 
										curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
								    }

									if(Method == L"POST")
									{
										curl_easy_setopt(curl, CURLOPT_POSTFIELDS, Content.data());
									}

									if(FollowLocatiion)
									{
										curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
									}

									curl_easy_setopt(curl, CURLOPT_TIMEOUT, Timeout);
									
									Result = curl_easy_perform(curl);
									curl_easy_cleanup(curl);

									if(chunk)
									{
										curl_slist_free_all(chunk);
									}
								}

							},
							[this]
							{
								if(Result == CURLE_OK)
								{
									if(Recieved)
										Recieved();
								}
								else
								{
									if(Failed)
										Failed();
								}
							}
	);
}

size_t CHttpRequest::WriteCallback(void * contents, size_t size, size_t nmemb, void * userp)
{
	((CHttpRequest *)userp)->Stream.Write(contents, size * nmemb); // append((char *)contents, size * nmemb);
	return size * nmemb;
}
