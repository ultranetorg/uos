#include "StdAfx.h"
#include "Nexus.h"

using namespace uos;

CNexus::CNexus(CCore * l, CXonDocument * config)
{
	Core = l;

	auto d = Core->GetPathTo(ESystemPath::Root, L"Nexus.xon");
	auto c = Core->MapToDatabase(UOS_MOUNT_USER_GLOBAL L"\\Nexus.xon");

	DefaultConfig = new CTonDocument(CXonTextReader(&CFileStream(d, EFileMode::Open)));

	if(CNativePath::IsFile(c))
	{
		Config = new CTonDocument(CXonTextReader(&CFileStream(d, EFileMode::Open)), CXonTextReader(&CFileStream(c, EFileMode::Open)));
	}
	else
	{
		Config = new CTonDocument(CXonTextReader(&CFileStream(d, EFileMode::Open)));
	}

	Diagnostic = Core->Supervisor->CreateDiagnostics(GetClassName());
	Diagnostic->Updating += ThisHandler(OnDiagnosticUpdating);
	

//		SetDllDirectory(Core->GetPathTo(ESystemFolder::Executables, L".").c_str());
	wchar_t b[32768];
	GetEnvironmentVariable(L"PATH", b, _countof(b));
	CString e_path = Core->GetPathTo(ESystemPath::Common, L".") + L";" + CString(b);
	SetEnvironmentVariable(L"PATH", e_path.c_str());		

	ExitHotKeyId	= Core->RegisterGlobalHotKey(MOD_ALT|MOD_CONTROL,			VK_ESCAPE, ThisHandler(ProcessHotKey));
	SuspendHotKeyId	= Core->RegisterGlobalHotKey(MOD_ALT|MOD_CONTROL|MOD_SHIFT,	VK_ESCAPE, ThisHandler(ProcessHotKey));

	Level2.Core		= Core;
	Level2.Log		= Core->Log;
	Level2.Nexus	= this;

	DirectoryPath		= Core->MapToDatabase(L".");
	ObjectTemplatePath	= Core->GetPathTo(ESystemPath::Root, L"Object.xon");

	Core->AddWorker(this);

	StartServers();
	
	Core->RegisterExecutor(this);

	Initialized();
}

CNexus::~CNexus()
{
	StopServers();
	
	Core->RemoveWorker(this);

	delete Dms;
	delete Fdn;

	Config->Save(&CXonTextWriter(&CFileStream(Core->MapToDatabase(UOS_MOUNT_USER_GLOBAL L"\\Nexus.xon"), EFileMode::New), false), DefaultConfig);
	delete Config;
	delete DefaultConfig;


	Diagnostic->Updating -= ThisHandler(OnDiagnosticUpdating);
	Core->LevelDestroyed(2, this);
	Core->Log->ReportMessage(this, L"-------------------------------- Nexus destroyed --------------------------------");
}

void CNexus::OnDiagnosticUpdating(CDiagnosticUpdate & a)
{
	for(auto s : Servers)
	{
		Diagnostic->Add(s->Info->Url.Server);

		for(auto o : s->Objects)
		{
			Diagnostic->Add(CString::Format(L"  %-55s %3d", o->Url.Object, o->GetRefs()));
			
			//Diagnostic->Append(CString::Format(L"%-s", CString::Join(o->Users, [](auto & i){ return CString::Format(L"%s(%d)", i.first, i.second.size()); }, L",") ));
		}
	}
}

void CNexus::StartServers()
{
	UpdateStatus = L"Checking for updates ...";

	auto inf = new CServerInfo();
	inf->HInstance		= null;
	inf->Xon			= null;
	inf->Origin			= CUrl(UOS_FILE_SYSTEM_ORIGIN);
	inf->Url			= CUsl(/*Core->DatabaseId + L".ultranet"*/L"", UOS_FILE_SYSTEM/*, UOS_FILE_SYSTEM_ORIGIN*/);
	inf->Role			= L"Server";
	inf->Installed		= true;
	Infos.push_back(inf);
	Servers.push_back(Storage = new CStorage(&Level2, inf));
	
	for(auto i : Config->Many(L"Nexus/Server"))
	{
		auto inf = new CServerInfo();
		inf->Xon			= i;
		inf->Origin			= i->Get<CUrl>(L"Origin");
		inf->Url			= CUsl(/*Core->DatabaseId + L".ultranet",*/ L"", i->AsString()/*, i->Get<CUrl>(L"Origin").ToString()*/);
		inf->Role			= i->Get<CString>(L"Role");
		inf->Installed		= i->Get<CBool>(L"IsInitialized").Value;
		inf->InstallPath	= L"/" UOS_MOUNT_SERVER  L"/" + inf->Url.Server;
		inf->ObjectsPath	= Storage->MapPath(UOS_MOUNT_USER_GLOBAL, i->AsString());

		Infos.push_back(inf);
	}

	Storage->Start(EStartMode::Start);

	Fdn	= new CFdn(&Level2);
	Dms	= new CDms(&Level2, Fdn);

	SetDllDirectories();

	for(auto i : Infos)
	{
		if(i == Storage->Info)
			continue;
			
		auto s = GetServer(i->Url.Server);
	}

	for(auto i : Infos)
	{
		auto s = GetServer(i->Url.Server);

		if(!i->Installed)
		{
			Storage->CreateMounts(i);
			s->Start(EStartMode::Installing);
			i->Xon->One(L"IsInitialized")->Set(true);
		}
		else
		{
			s->Start(EStartMode::Start);
		}
	}
	
	Dms->FindReleases(Core->Product.Name.ToLower(), 
					Core->Product.Platform.ToLower(),	
					[this](CArray<uint256> & builds)
					{
						if(!builds.empty())
						{
							Dms->GetRelease(Core->Product.Name.ToLower(), 
											builds.back(),
											[this](auto p, auto v, auto cid)
											{
												if(Core->Product.Version < v)
												{
													auto b = new CProductRelease();
													b->Product	= Core->Product.Name.ToLower();
													b->Version	= v;
													b->Cid		= cid;

													NewReleases.push_back(b);

													Core->Log->ReportMessage(this, L"Latest release: %s %s %s", p, v.ToString(), cid);
												}
												else
													UpdateStatus = L"No updates found";

												UpdateStatusChanged();
											});
						}
						else
							Core->Log->ReportWarning(this, L"No %s release found", Core->Product.HumanName);
					});



	Core->Log->ReportMessage(this, L"-------------------------------- Nexus created --------------------------------");
	Core->LevelCreated(2, this);
}

void CNexus::StopServers()
{
	Stopping();

	while(auto s = Servers.Last())
	{
		StopServer(s);
	}

	for(auto si : Infos)
	{
		if(si->HInstance)
		{
			#ifndef _DEBUG
			FreeLibrary(si->HInstance);
			#endif
		}
		delete si;
	}

	Infos.clear();
}

void CNexus::StopServer(CServer * s)
{
	for(auto & i : s->Protocols)
	{
		if(i.second)
		{
			Break(s->Url, i.first);
		}
	}

	Servers.Remove(s);

	auto si = Infos.Find([s](auto i){ return i->Url == s->Url; });
	
	if(si->HInstance)
	{
		auto stop = (FStopUosHub)GetProcAddress(si->HInstance, "StopUosServer");
		stop();
	}
	else
		delete s;
}

bool CNexus::HasInstantWork()
{
	return !RestartCommand.empty();
}

void CNexus::DoInstant()
{
	if(!RestartCommand.empty())
	{
		StopServers();
		StartServers();
		RestartCommand.clear();
	}
}

void CNexus::Restart(CString const & cmd)
{
	RestartCommand = cmd;
}

CString CNexus::GetExecutable(CString const & spath)
{
	auto s = Storage->OpenReadStream(CPath::Join(spath, L"Server.xon"));

	auto & d = CTonDocument(CXonTextReader(s));

	Storage->Close(s);

	for(auto j : d.Many(L"Executable"))
	{
		if(true /*j->Get<CString>(L"Platform") == Core->ProductInfo.Platform && j->Get<CString>(L"Build") == Core->ProductInfo.Build*/)
		{
			auto file = j->Get<CString>(L"File");
							
			return CPath::Join(spath, file);
		}
	}

	throw CException(HERE, L"Executable not found");
}


void CNexus::SetDllDirectories()
{
	wchar_t b[32767];
	GetEnvironmentVariable(L"PATH", b, _countof(b));

	CString path = b;

	//SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

	for(auto i : Infos)
	{
		if(!i->InstallPath.empty())
		{
			auto dll = GetExecutable(i->InstallPath);
			auto d = CNativePath::GetDirectoryPath(Storage->UniversalToNative(dll));
			path += L";" + d;
		}
	}

	SetEnvironmentVariable(L"PATH", path.data());
}

CServer * CNexus::GetServer(CString const & name)
{
	auto info = Infos.Find([name](auto i){ return i->Url.Server == name; });

	if(!info)
	{
		return null;
	}

	auto s = Servers.Find([info](auto i){ return i->Info == info; });

	if(s)
	{
		return s;
	}

	auto dllpath = Storage->UniversalToNative(GetExecutable(info->InstallPath));

	if(!CNativePath::IsFile(dllpath))
	{
		throw CException(HERE, CString::Format(L"Server dll not found: %s", dllpath));
	}

	if(	CVersion::GetStringInfo(dllpath, L"Platform")	!= UOS_PROJECT_TARGET_PLATFORM ||
		CVersion::GetStringInfo(dllpath, L"Build")		!= UOS_PROJECT_CONFIGURATION)
	{
		throw CException(HERE, CString::Format(L"Server has wrong Platform|Build: %s", dllpath));
	}

	info->HInstance = LoadLibrary(dllpath.c_str());
	info->Version = CVersion::FromModule(info->HInstance);

	if(info->HInstance == null)
	{
		throw CLastErrorException(HERE, GetLastError(), L"DLL loading error: %s ", dllpath.c_str());
	}
	
	auto f = (FStartUosHub)GetProcAddress(info->HInstance, "StartUosServer");


	if(f == null)
	{
		FreeLibrary(info->HInstance);
		throw CException(HERE, CString::Format(L"Interface not found: %s", dllpath));
	}
			
	s = f(&Level2, info);
		
	if(s == null)
	{
		throw CException(HERE, CString::Format(L"Unable to initilize system: %s", dllpath));
	}

	if(auto r = GetRegistry(s->Url, L"Interfaces"))
	{
		for(auto i : r->Children)
		{
			s->Protocols[i->Name] = null;
		}
	}

	Core->Log->ReportMessage(this, CString::Format(L"Server loaded: %s", info->Url.Server));
		
	Servers.push_back(s);

	return s;
}

CConnection CNexus::Connect(IType * who, CUsl & u, CString const & p)
{
	auto s = GetServer(u.Server);

	if(!s)
		return CConnection();

	if(s->Protocols.Contains(p))
	{
		if(s->Protocols(p) == null)
		{
			auto imp = s->Connect(p);
			if(imp)
			{
				s->Protocols[p] = imp;
				Core->Log->ReportMessage(this, L"Server connected: %-50s %s", s->Url.Server, p);
			}
		}
	
		if(s->Protocols[p] != null)
		{
			s->Users[p].push_back(who);
			return CConnection(who, s, s->Protocols[p], p);
		}
	}
	
	return CConnection();
}

CConnection CNexus::Connect(IType * who, CString const & p)
{
	auto o = FindImplementators(p);
	if(!o.empty())
	{
		return Connect(who, o.front(), p);
	}
	else
		return CConnection();
}


CList<CConnection> CNexus::ConnectMany(IType * who, CString const & p)
{
	if(p.empty())
	{
		throw CException(HERE, L"Protocol must be specified");
	}

	CList<CConnection> cc;

	for(auto i : Servers)
	{
		if(i->Protocols.Contains(p))
		{
			cc.push_back(Connect(who, i->Url, p));
		}
	}
	
	return cc;
}

void CNexus::Disconnect(CConnection & c)
{
	c.Server->Users[c.ProtocolName].Remove(c.Who);
		
	c.Protocol = null;
	c.Server = null;
	c.Who = null;
}

void CNexus::Disconnect(CList<CConnection> & cc)
{
	for(auto c : cc)
	{
		Disconnect(c);
	}
}

void CNexus::Break(CUsl & sys, CString const & pr)
{
	Core->Log->ReportMessage(this, L"Disconnecting: %s %s", sys.Server, pr);

	auto s = GetServer(sys.Server);

	if(s->Protocols.Contains(pr) && s->Protocols[pr] != null)
	{
		s->Disconnecting(s, s->Protocols[pr], const_cast<CString &>(pr));
		s->Disconnect(s->Protocols[pr]);
	
		s->Protocols[pr] = null;
	}
}

CList<CUsl> CNexus::FindImplementators(CString const & pr)
{
	CList<CUsl> o;
	
	for(auto i : Servers)
	{
		if(i->Protocols.Contains(pr))
		{
			o.push_back(i->Url);
		}
	}
	return o;
}

void CNexus::ProcessHotKey(int64_t id)
{
	if(id == ExitHotKeyId)
	{
		Core->Exit();
	}
	if(id == SuspendHotKeyId)
	{
		if(Core->SuspendStatus)
		{
			Core->Resume();
		}
		else
		{
			Core->Suspend();
		}
	}
}

void CNexus::Execute(const CUrq & u, CExecutionParameters * p)
{
	if(u.Protocol == CUol::Protocol)
	{
		if(u.GetSystem() == Storage->CServer::Url.Server)
		{
			ShellExecute(null, L"open", Storage->UniversalToNative(u.GetObject()).data(), NULL, NULL, SW_SHOWNORMAL);
		}
		else
		{
			auto ep = Connect<IExecutorProtocol>(this, CUsl(u), EXECUTOR_PROTOCOL);

			if(ep && ep->CanExecute(u))
			{
				ep->Execute(u, p);
				Disconnect(ep);
				return;
			}

			auto pp = ConnectMany<IExecutorProtocol>(this, EXECUTOR_PROTOCOL);

			for(auto i : pp)
			{
				if(i->CanExecute(u))
				{
					i->Execute(u, p);
					break;
				}
			}

			Disconnect(pp);
		}
	}
	else
	{
		ShellExecute(null, L"open", u.ToString().data(), NULL, NULL, SW_SHOWNORMAL);
	}
}

CMap<CServerInfo *, CXon *> CNexus::GetRegistry(CString const & path)
{
	CMap<CServerInfo *, CXon *> l;

	for(auto i : Infos)
	{
		if(i != Storage->Info)
		{
			if(auto r = GetRegistry(i->Url, path))
			{
				l[i] = r;
			}
		}
	}

	return l;
}

CXon * CNexus::GetRegistry(CUsl & s, CString const & path)
{
	auto info = Infos.Find([s](auto i){ return i->Url == s; });

	if(!info->Registry)
	{
		auto p = Storage->MapPath(s, L"Server.registry");

		if(auto s = Storage->OpenReadStream(p))
		{
			info->Registry = new CTonDocument(CXonTextReader(s));
			Storage->Close(s);
		} 
	}

	return info && info->Registry ? info->Registry->One(path) : null;
}
