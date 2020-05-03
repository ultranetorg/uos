#include "StdAfx.h"
#include "Core.h"
#include "Mmc.h"
#include "FileStream.h"
#include "File.h"
#include "StaticArray.h"

using namespace uos;
using namespace std::experimental::filesystem;

static wchar_t SupervisorDirectory[32768];

wchar_t * uos::GetSupervisorDataFolder()
{
	return SupervisorDirectory;
}

void SetSupervisingDirectory(CString const & path)
{
	//TCHAR szTmp[MAX_PATH];
	//GetTempPath(_countof(szTmp), szTmp); 
	//
	//PathCombine(SupervisorDirectory, szTmp,	(CString(company_name)+L"-"+product_name+L"-"+supervisor_folder+L"-"+target_platform).c_str());

	wcscpy_s<>(SupervisorDirectory, path.data());

	CreateDirectory(SupervisorDirectory,	null);

	TCHAR toDelete[MAX_PATH];
	ZeroMemory(toDelete, sizeof(toDelete));
	PathCombine(toDelete, SupervisorDirectory, L"*.*");
	//	wcscat_s(toDelete, _countof(toDelete), L'\0');

	SHFILEOPSTRUCT fo;
	ZeroMemory(&fo, sizeof(SHFILEOPSTRUCT));
	fo.pFrom	= toDelete;
	fo.wFunc	= FO_DELETE;
	fo.fFlags	= FOF_SILENT|FOF_NOERRORUI|FOF_NOCONFIRMATION;
	int e = SHFileOperation(&fo);
}


CCore::CCore(CSupervisor * s, HINSTANCE instance, wchar_t ** argv, int argc, const wchar_t * supervisor_folder, const wchar_t * root_from_exe, const wchar_t * coredir, CProductInfo & pi)
{
	Supervisor			= s;
	Core				= this;
	CreationInstance	= instance;
	Product			= pi;
	Os					= new COsNT();

	CoInitialize(NULL);
//	curl_global_init(CURL_GLOBAL_DEFAULT);

	wchar_t p[32768];
	GetModuleFileNameW(CreationInstance, p, _countof(p));

	SupervisorName		= supervisor_folder;
	LaunchPath			= CNativePath::Canonicalize(p);
	LaunchFolder		= CNativePath::GetDirectoryPath(LaunchPath);
	RootPath			= CNativePath::Join(LaunchFolder, root_from_exe);
	CorePath			= CNativePath::Join(RootPath, coredir);
	DatabaseObject		= GetClassName() + L"/Database";

	//auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |PROCESS_VM_READ, FALSE, GetCurrentProcessId());
	//GetModuleFileNameEx(hProcess, LoadLibrary(L"Uos.dll"), szModName, sizeof(szModName) / sizeof(TCHAR))

	GetCurrentDirectory(_countof(p), p);
	FrameworkDirectory = p;

	Timings.TotalTicks			= 0;
	Timings.MessageP1SCounter	= 0;
	Timings.EventP1SCounter		= 0;
	
	Log			= Supervisor->MainLog = Supervisor->CreateLog(L"Core");
	Diagnostics	= Supervisor->CreateDiagnostics(L"Core");
	Diagnostics->Updating += ThisHandler(OnDiagnosticsUpdate);

	Log->ReportMessage(this, L"Welcome, %s", Os->GetUserName());
	Log->ReportMessage(this, L"%s", Product.ToString(L"NVSPB"));
	Log->ReportMessage(this, L"OS: %s", Os->GetVersion().ToString().c_str());
	Log->ReportMessage(this, L"User Admin?: %s", CSecurity().IsUserAdmin()? L"y" : L"n");
	Log->ReportMessage(this, L"Root directory: %s", RootPath);


	if(argc > 1)
	{
		for(int i=1; i<argc; i++)
		{
			Commands.push_back(CUrq(argv[i]));
			Log->ReportMessage(this, L"Command: %s", argv[i]);
		}
	}

	HANDLE hAccessToken = NULL;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hAccessToken))
	{
		Os->SetPrivilege(hAccessToken, SE_BACKUP_NAME, TRUE);
		Os->SetPrivilege(hAccessToken, SE_CREATE_GLOBAL_NAME, TRUE);
	}
	
////
	if(CLocalFile::Exists(CNativePath::Join(CorePath, L"Database.xon")) == false)
	{
		CommitDatabase = false;
		return;
	}

	DBConfig = new CTonDocument(CXonTextReader(&CFileStream(CNativePath::Join(CorePath, L"Database.xon"), EFileMode::Open)));
	
	auto modes = DBConfig->Many(L"Database");
	CXon * xdb = null;

	for(auto i : modes)
	{
		auto xfor = i->One(L"For");
		
		if(xfor)
		{
			CString dbg = xfor->One(L"Debugger")->Get<CString>();
			bool debug =	(dbg == L"On"	&& (IsDebuggerPresent()==TRUE)) || 
							(dbg == L"Off"	&& (IsDebuggerPresent()==FALSE)) || 
							dbg == L"";

			bool build =	(xfor->Get<CString>(L"BuildConfig") == Product.Build) ||
							(xfor->Get<CString>(L"BuildConfig") == L"");

			bool machine =	(xfor->One(L"Machine")->AsString() == Os->ComputerName()) || (xfor->One(L"Machine")->AsString() == L"");

			if(debug && build && machine)
			{
				xdb = i;
				break;
			}
		}
	}

	if(xdb)
	{
		UserPath		= ResolveConstants(xdb->Get<CString>(L"User"));
		ServersPath		= ResolveConstants(xdb->Get<CString>(L"Servers"));
		CommonPath		= ResolveConstants(xdb->Get<CString>(L"Common"));
	}
	else
		throw CException(HERE, L"Database config is incorrect");
	
	InitializeUnid();
	
	int bufsize = sizeof(CUosNodeInformation);
	CString fmname = L"Global\\" + Product.Name + L"-" + Unid;

	HInformation = CreateFileMapping(INVALID_HANDLE_VALUE,   // use paging file
									 NULL,                   // default security
									 PAGE_READWRITE,         // read/write access
									 0,                      // maximum object size (high-order DWORD)
									 bufsize,                // maximum object size (low-order DWORD)
									 fmname.c_str());        // name of mapping object

	if(HInformation == NULL)
	{
		throw CLastErrorException(HERE, GetLastError(), L"Could not create file mapping: %s", fmname.c_str());
	}

	auto status = GetLastError();

	auto info = (CUosNodeInformation *)MapViewOfFile(HInformation, FILE_MAP_ALL_ACCESS, 0, 0, bufsize);

	if(info == NULL)
	{
		throw CLastErrorException(HERE, GetLastError(), L"Could not map view of file: %s", fmname.c_str());
	}

	if(status == ERROR_SUCCESS)
	{
		Information = info;
		Information->ProcessId = GetCurrentProcessId();
		Information->MainThreadId = GetCurrentThreadId();
	}
	else if(status == ERROR_ALREADY_EXISTS)
	{
		CString c = CString::Join(Commands, [this](auto i){ return i.ToString(); }, L" ");;

		COPYDATASTRUCT cd;
		cd.dwData = 0;
		cd.lpData = (void *)c.data();
		cd.cbData = DWORD(c.size()) * 2 + 2;

		auto e = SendMessage(info->Mmc, WM_COPYDATA, (WPARAM)0, (LPARAM)&cd);
		return;
	}

	static int memory=0;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPWSTR)&memory, &LocationInstance);

	Timer.Restart();
	TickTimer.Restart();
	SecTimer.Restart();

	InitializeDatabase();
	
	if(!DatabaseInitialized)
	{
		return;
	}

	Supervisor->StartWriting(SupervisorDirectory);
	PCCycle =  new CPerformanceCounter();
	

	Os->RegisterUrlProtocol(UOS_OBJECT_PROTOCOL, UO_NAME, LaunchPath);

	Log->ReportMessage(this, L"Supervisor directory: %s", Supervisor->Directory);
	Log->ReportMessage(this, L"Database source:      %s", SourceDirectory);
	Log->ReportMessage(this, L"Database destination: %s", DestinationDirectory);
	Log->ReportMessage(this, L"Database work:        %s", WorkDirectory);

	LoadParameters();
	SetDllDirectory(LaunchFolder.data());

	for(auto i : Commands)
	{
		Execute(i, null);
	}

	Mmc = new CMmc(this, Config);

	Initialized = true;

	Log->ReportMessage(this, L"-------------------------------- Core created --------------------------------");

}
	
CCore::~CCore()
{
	delete Os;

	if(Initialized)
	{
		//for(auto i : RunningThreads)
		//	delete i;
		//
		//for(auto i : PendingThreads)
		//	delete i;

		delete Mmc;

		Config->Save(&CXonTextWriter(&CFileStream(Core->MapToDatabase(UOS_MOUNT_USER_LOCAL L"\\Core.xon"), EFileMode::New), false), DefaultConfig);

		Supervisor->StopWriting();

		ShutdownDatabase();

		delete Config;
		delete DefaultConfig;
		delete DBConfig;
		delete PCCycle;

		if(Information)
		{
			UnmapViewOfFile(Information);
			CloseHandle(HInformation);
		}

		if(!RestartCommand.empty())
		{
			STARTUPINFO info={sizeof(info)};
			PROCESS_INFORMATION processInfo;

			wchar_t cmd[32768] = {};
			wcscpy_s(cmd, _countof(cmd), (L"\"" + LaunchPath + L"\" " + RestartCommand).data());

			SetDllDirectory(FrameworkDirectory.data());
			SetCurrentDirectory(FrameworkDirectory.data());

			if(!CreateProcess(null, cmd, null, null, true, /*IsDebuggerPresent() ? DEBUG_PROCESS : 0*/0, null, FrameworkDirectory.data(), &info, &processInfo))
			{
				auto e = GetLastError();
			}
		}
		
	}
	//curl_global_cleanup();
}

void CCore::InitializeUnid()
{
//	CNativeDirectory::CreateAll(CNativePath::GetDirectoryPath(UserPath), true);

	auto pattern = CNativePath::GetDirectoryName(UserPath);

	CString name;

	for(auto i : CNativeDirectory::Find(CNativePath::GetDirectoryPath(UserPath), pattern + L"-*", EDirectoryFlag(DirectoriesOnly|SkipServiceElements)))
	{
		name = i.Path;
	}
	
	if(!name.empty())
	{
		for(auto i : CNativeDirectory::Find(CNativePath::GetDirectoryPath(UserPath), name + L"\\*", EDirectoryFlag(DirectoriesOnly|SkipServiceElements)))
		{
			if(CInt32::Valid(i.Path))
			{
				Commits.push_back(i.Path);
				auto c = CInt32::Parse(i.Path);
				LastCommit = max(LastCommit, c);
			}
		}
	}

	if(LastCommit == -1)
	{
#ifdef _DEBUG
		Unid = L"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
#else
		Unid = CBase58::Encode(CGuid::New256());
#endif		
		CNativeDirectory::CreateAll(UserPath + L"-" + Unid, true);
	}
	else
	{
		std::wregex r(CNativePath::EscapeRegex(pattern) + L"-(.+)");
		auto words_begin = std::wsregex_iterator(name.begin(), name.end(), r);

		Unid = (*words_begin)[1].str();
	}

}

void CCore::InitializeDatabase()
{
	auto fbase = UserPath + L"-" + Unid + L"\\%08d";

	if(HasCommand(L"Action", L"Rollback") && LastCommit >= 0)
	{
		CNativeDirectory::Delete(CString::Format(fbase, LastCommit));
		LastCommit--;
		Log->ReportWarning(this, L"Database rollback requested");
	}

	while(true)
	{
		SourceDirectory			= CString::Format(fbase, LastCommit);
		DestinationDirectory	= CString::Format(fbase, LastCommit + 1);

		if(DBConfig->Get<CString>(L"Increment") == L"n" && LastCommit >= 0)
		{
			DestinationDirectory	= CString::Format(fbase, LastCommit);
			Log->ReportWarning(this, L"No increment");
		}

		auto workbasename = L"!" + Product.Name + L"-" + Unid + L"-0";
		auto workbasepath = CNativePath::Join(CNativePath::GetTmp(), workbasename);

		WorkDirectory = workbasepath + L"-" + CGuid::Generate64();
		WorkTmpFolder = WorkDirectory + L"\\" + UOS_MOUNT_SERVER_TMP;

		try
		{
			for(auto i : CNativeDirectory::Find(CNativePath::GetTmp(), workbasename + L"-*", EDirectoryFlag(EDirectoryFlag::AsPath|EDirectoryFlag::DirectoriesOnly)))
			{
				CNativeDirectory::Delete(i.Path);
			}

			CNativeDirectory::Create(WorkDirectory);
			CNativeDirectory::Create(WorkTmpFolder);
			SetSupervisingDirectory(WorkDirectory + L"\\" + SupervisorName);


			CNativeDirectory::Create(WorkDirectory + L"\\" + UOS_MOUNT_USER_LOCAL);
			CNativeDirectory::Create(WorkDirectory + L"\\" + UOS_MOUNT_USER_GLOBAL);

			if(CNativeDirectory::Exists(SourceDirectory))
			{
				CNativeDirectory::Copy(SourceDirectory + L"\\" + UOS_MOUNT_USER_LOCAL, WorkDirectory + L"\\" + UOS_MOUNT_USER_LOCAL);
				CNativeDirectory::Copy(SourceDirectory + L"\\" + UOS_MOUNT_USER_GLOBAL, WorkDirectory + L"\\" + UOS_MOUNT_USER_GLOBAL);

				if(DBConfig->Get<CString>(L"Increment") == L"n")
				{
					CNativeDirectory::Delete(DestinationDirectory + L"\\" + UOS_MOUNT_USER_LOCAL);
					CNativeDirectory::Delete(DestinationDirectory + L"\\" + UOS_MOUNT_USER_GLOBAL);
				}
			}
		}
		catch(CException &)
		{
			MessageBox(null, (L"Failed to initialize database. Some files may still be locked by hanging processes. This is often caused by incorrect shutdown of " + Product.HumanName).data(), L"Error", MB_OK|MB_ICONERROR);
			abort();
		}


		auto c = MapToDatabase(UOS_MOUNT_USER_LOCAL L"\\Core.xon");
	
		auto d = GetPathTo(ESystemPath::Root, L"Core.xon");
		DefaultConfig = new CTonDocument(CXonTextReader(&CFileStream(d, EFileMode::Open)));
	
		if(CNativePath::IsFile(c))
		{
			Config = new CTonDocument(CXonTextReader(&CFileStream(d, EFileMode::Open)), CXonTextReader(&CFileStream(c, EFileMode::Open)));
	
			if(Config->Get<CInt32>(L"Database/Version") == DBConfig->Get<CInt32>(L"Version"))
			{
				DatabaseInitialized = true;
			}
			else
			{
				if(MessageBox(null,(L"Your profile needs to be reset to a default state.\n\n"
									L"Ultranet profile " + Unid + L" is not compatible with this version of " + Product.HumanName + L" and migration is not supported yet.\n\n"
									"Confirm that you are agree to recreate profile from scratch.\n\n"
									"All existing data of the previous profile will be lost.").data(), (Product.HumanName + L": Warning").data(), MB_OKCANCEL|MB_ICONEXCLAMATION) == IDOK)
				{
					CNativeDirectory::Delete(WorkDirectory);

					for(auto i : Commits)
					{
						CNativeDirectory::Delete(CNativePath::Join(UserPath + L"-" + Unid, i));
					}

					delete DefaultConfig;
					delete Config;
					LastCommit = -1;

					Log->ReportWarning(this, L"Profile erased");
					continue;
				}
				else
				{
					DatabaseInitialized = false;
				}
			}
		}
		else
		{
			Config = new CTonDocument(CXonTextReader(&CFileStream(d, EFileMode::Open)));
			Config->One(L"Database/Version")->Set(DBConfig->Get<CInt32>(L"Version"));
			DatabaseInitialized = true;
		}
		break;
	}
}

void CCore::ShutdownDatabase()
{
	if(CommitDatabase)
	{
		CNativeDirectory::Create(DestinationDirectory);
		CNativeDirectory::Copy(WorkDirectory + L"\\" + UOS_MOUNT_USER_LOCAL, DestinationDirectory + L"\\" + UOS_MOUNT_USER_LOCAL);
		CNativeDirectory::Copy(WorkDirectory + L"\\" + UOS_MOUNT_USER_GLOBAL, DestinationDirectory + L"\\" + UOS_MOUNT_USER_GLOBAL);
		
		auto pattern = CNativePath::GetDirectoryName(UserPath);
		
		CArray<CString> dirs;

		for(auto i : CNativeDirectory::Find(CNativePath::GetDirectoryPath(UserPath), pattern + L"-" + Unid + L"\\*", EDirectoryFlag(DirectoriesOnly|SkipServiceElements)))
		{
			if(CInt32::Valid(i.Path))
			{
				dirs.push_back(i.Path);
			}
		}

		dirs.Sort([](auto a, auto b){ return a<b; });

		for(int i=0; i<dirs.Count(); i++)
		{
			if(int(dirs.size()) - i - 10 > 0)
			{
				CNativeDirectory::Delete(CNativePath::Join(UserPath + L"-" + Unid, dirs[i]));
			}
		}
	}
}

CString CCore::ResolveConstants(CString const & dir)
{
	CString userprofile;
	TCHAR szPath[MAX_PATH];
	
	if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, szPath))) 
	{
		userprofile = CNativePath::Join(szPath, Product.Name);
	}


	CString o = dir;

	o = o.Replace(L"{RootDirectory}",		RootPath);
	o = o.Replace(L"{LocalUsername}",		Os->GetUserName());
	o = o.Replace(L"{LocalUserProfile}",	userprofile);
	o = o.Replace(L"\\\\",					L"\\");

	return CNativePath::Canonicalize(o);
}
	
CString CCore::GetPathTo(ESystemPath folder, const CString & path)
{
	switch(folder)
	{
		case ESystemPath::Root:		return CNativePath::Join(RootPath, path);
		case ESystemPath::Servers:	return CNativePath::Join(ServersPath, path);
		case ESystemPath::Common:	return CNativePath::Join(CommonPath, path);
	}

	throw CException(HERE, L"Wrong folder type");
}

CString CCore::MapToDatabase(const CString & path)
{
	return CNativePath::Join(WorkDirectory, path);
}

CString CCore::MapToTmp(const CString & path)
{
	return CNativePath::Join(WorkTmpFolder, path);
}

void CCore::RegisterExecutor(ICommandExecutor * e)
{
	Executors.push_back(e);

	for(auto i : Commands)
	{
		Execute(i, null);
	}
}

void CCore::Execute(const CUrq & c, CExecutionParameters * p)
{
	if(c.Path == DatabaseObject)
	{
		if(c.Query.Contains(L"Action", L"Clear"))
		{
			///if(CPath::IsDirectory(WorkFolder))
			///{
			///	CNativeDirectory::Delete(WorkFolder);
			///	CreateDirectories(WorkFolder, true);
			///}
		}
	}
	else
		for(auto j : Executors)
		{
			j->Execute(c, p);
		}
}

void CCore::AddRestartCommand(CUrl const & c)
{
	RestartCommand = /*UOS_OBJECT_PROTOCOL  L"://" + DatabaseObject + L"?Backup=n " +*/ c.ToString();
}

void CCore::SetCommit(bool c)
{
	CommitDatabase = c;
}

void CCore::Run()
{
	while(!Exiting || !RunningThreads.empty() || !Jobs.empty())
	{
		MSG m;

		auto iw = Workers.Has([](auto i){ return i->HasInstantWork(); });

		{
			std::lock_guard<std::mutex> guard(JobsLock);
			if(!Jobs.empty())
				iw = true;
		}

		DWORD reason = MsgWaitForMultipleObjects(DWORD(Events.size()), Events.size() > 0 ? &Events.front() : null, false, iw ? 0 : MsgWaitDelay, QS_ALLEVENTS|QS_ALLINPUT|QS_ALLPOSTMESSAGE);

		if(WAIT_OBJECT_0 <= reason && reason < WAIT_OBJECT_0 + Events.size())
		{
			PCCycle->BeginMeasure();
			ProcessEvents(reason);
			PCCycle->EndMeasure();
		}
		else if(reason == WAIT_OBJECT_0 + Events.size())
		{
			while(PeekMessage(&m, null, 0, 0, PM_REMOVE))
			{
				PCCycle->BeginMeasure();
				if(!ProcessMessages(m))
				{
					TranslateMessage(&m); 
					DispatchMessage(&m); 
				}
				PCCycle->EndMeasure();

				ProcessIdle();
			}
		}

		ProcessIdle();
	}
}

void CCore::ProcessIdle()
{
	if(Initialized && !SuspendStatus)
	{
		PCCycle->BeginMeasure();

		//if(iw)
		{
			for(auto i : Workers)
			{
				if(i->HasInstantWork())
				{
					i->DoInstant();
				}
			}

			std::lock_guard<std::mutex> guard(JobsLock);
			auto jobs = Jobs;

			for(auto i : jobs)
			{
				if(i->Work() == true)
				{
					delete i;
					Jobs.Remove(i);
				}
			}
		}

		if(TickTimer.IsElapsed(1.f/IdleCps, true))
		{
			for(auto i : IdleWorkers)
			{
				i->DoIdle();
			}
		}
		
		PCCycle->EndMeasure();
		
		if(SecTimer.IsElapsed(1.f, true))
		{
			SecTick();

			Timings.MessageP1SCounter	= 0;
			Timings.EventP1SCounter		= 0;

			PCCycle->Reset();

			for(auto i : PerformanceCounters)
				i->Reset();
		}
	}
	//Log->ReportDebug(this, L"kkk");

	Supervisor->Cycles++;
}

void CCore::ProcessCopyData(COPYDATASTRUCT * cd)
{
	Log->ReportMessage(this, L"Instance message recieved: %s", cd->lpData);
	Execute(CUrq((wchar_t *)cd->lpData), null);
}
	
void CCore::ProcessEvents(int i)
{
	if(!SuspendStatus && Initialized)
	{
		Timings.EventP1SCounter++;

		// run next pending thread
		auto t = RunningThreads.Find([this, i](auto j){ return j->Handle == Events[i]; });

		if(t)
		{
			t->Exited();
			RunningThreads.remove(t);
			delete t;

			if(!PendingThreads.empty())
			{
				auto t = PendingThreads.front();
				PendingThreads.remove(t);
				RunningThreads.push_back(t);

				t->Start();
			}
		}
		else
		{
			EventHandlers[i]();
		}
	}
}

bool CCore::ProcessMessages(MSG & m)
{
	if(!SuspendStatus && Initialized)
	{
		Timings.MessageP1SCounter++;

		for(auto i : NmHandlers)
		{
			if(i->ProcessMessage(&m))
			{
				return true;
			}
		}
		if(m.message == WM_HOTKEY)
		{
			auto id = m.wParam;
			if(id < HotKeys.size())
			{
				if(HotKeys[id])
				{
					HotKeys[id](id);
				}
			}
		}
		return false;
	}
	return false;
}	
/*

void CCore::ProcessException(CException & e)
{
	NeedErrorReporting	= true;
	Log->ReportException(this, e);
	Supervisor->Commit();
}
	
void CCore::ProcessAttentionException(CAttentionException & e)
{
	Suspend();
	Log->ReportError(this, L"Attention:");
	Log->ReportException(this, e);
}
*/

CThread	* CCore::RunThread(const CString & name, std::function<void()> main, std::function<void()> exit)
{
	CThread * t = new CThread(this);
	t->Name = name;
	t->Main = main;
	t->Exited = exit;
	
	if(Events.size() < MAXIMUM_WAIT_OBJECTS - 1)
	{
		RunningThreads.push_back(t);
		t->Start();
	} 
	else
	{
		PendingThreads.push_back(t);
	}

	return t;
}

void CCore::LoadParameters()
{
	auto p = Config->One(L"Level1");
	IsAdministrating	= p->One(L"Admin")->AsBool();
	MsgWaitDelay		= p->One(L"MsgWaitDelay")->AsInt32();
	IdleCps				= p->One(L"IdleCps")->AsInt32();
}

void CCore::RegisterEvent(HANDLE e, std::function<void()> fired)
{
	for(auto i : Events)
	{
		if(i == e)
		{
			throw CException(HERE, L"Event already registered");
		}
	}
	
	Events.push_back(e);
	EventHandlers.push_back(fired);
}

void CCore::UnregisterEvent(HANDLE e)
{
	auto j = EventHandlers.begin();
	for(auto i = Events.begin(); i != Events.end(); i++)
	{
		if(*i == e)
		{
			Events.erase(i);
			EventHandlers.erase(j);
			break;
		}
		j++;
	}
}

void CCore::RegisterNativeMessageHandler(INativeMessageHandler * h)
{
	NmHandlers.push_back(h);
}

void CCore::UnregisterNativeMessageHandler(INativeMessageHandler * h)
{
	NmHandlers.remove(h);
}

int CCore::RegisterGlobalHotKey(int modifier, int vk, std::function<void(int64_t)> h)
{
	bool foundFree = false;
	int id = 0;

	for(; id<(int)HotKeys.size(); id++)
	{
		if(!HotKeys[id])
		{
			foundFree = true;
			break;
		}
	}

	if(!foundFree)
	{
		id = (int)HotKeys.size();
	}
		
	if(::RegisterHotKey(null, id, modifier, vk) != 0)
	{
		HotKeys.resize(HotKeys.size() + 1);
		HotKeys[id] = h;
		return id;
	}
	else
	{
		Log->ReportWarning(this, L"RegisterGlobalHotKey faield, LastErorr: %d", GetLastError());
	}
	return -1;
}

void CCore::UnregisterGlobalHotKey(int id)
{
	::UnregisterHotKey(null, id);
	HotKeys[id] = std::function<void(int64_t)>();
}

void CCore::AddPerformanceCounter(IPerformanceCounter * pc)
{
	PerformanceCounters.push_back(dynamic_cast<CPerformanceCounter *>(pc));
}

void CCore::RemovePerformanceCounter(IPerformanceCounter * pc)
{
	PerformanceCounters.Remove(dynamic_cast<CPerformanceCounter *>(pc));
}

void CCore::AddJob(IType * owner, const CString & name, std::function<bool()> work)
{
	auto j = new CJob();
	j->Owner = owner;
	j->Name = name;
	j->Work = work;

	if(GetCurrentThreadId() != Information->MainThreadId)
	{
		std::lock_guard<std::mutex> guard(JobsLock);
		Jobs.push_back(j);
	}
	else
	{
		Jobs.push_back(j);
	}
}

void CCore::CancelJobs(IType * owner)
{
	for(auto i = Jobs.begin(); i != Jobs.end(); )
	{
		if((*i)->Owner == owner)
		{	
			delete *i;
			Jobs.erase(i++);
		}
		else
			i++;
	}
}

void CCore::AddWorker(IWorker * w)
{
	Workers.push_back(w);
}

void CCore::RemoveWorker(IWorker * w)
{
	Workers.Remove(w);
}

void CCore::AddWorker(IIdleWorker * w)
{
	IdleWorkers.push_back(w);
}

void CCore::RemoveWorker(IIdleWorker * w)
{
	IdleWorkers.Remove(w);
}

void CCore::Exit()
{
	Exiting = true;
	ExitQueried();
}

void CCore::Suspend()
{
	if(!SuspendStatus)
	{
		SuspendStatus = true;
		Suspended();
		Log->ReportWarning(this, L"Suspended");
	}
}

void CCore::Resume()
{
	if(SuspendStatus)
	{
		SuspendStatus = false;
		Resumed();
		Log->ReportWarning(this, L"Resumed");
	}
}

void CCore::OnDiagnosticsUpdate(CDiagnosticUpdate & a)
{
	Diagnostics->Add(L"CPU %%     : %10.0f", Timings.CpuMonitor.GetUsage());
	Diagnostics->Add(L"Cycle Max : %10.0f", 1e6 * PCCycle->Max);
	Diagnostics->Add(L"Cycle Min : %10.0f", 1e6 * PCCycle->Min);
	Diagnostics->Add(L"CPS       : %10lld", PCCycle->GetMeasures());
	Diagnostics->Add(L"EPS       : %10d", Timings.EventP1SCounter);
	Diagnostics->Add(L"MPS       : %10d", Timings.MessageP1SCounter);
	

	_CrtMemState s;
	_CrtMemCheckpoint(&s);
	Diagnostics->Add(L"Allocations");
	Diagnostics->Add(L"   Free   :  %10d", s.lCounts[0]);
	Diagnostics->Add(L"   Normal :  %10d", s.lCounts[1]);
	Diagnostics->Add(L"   Crt    :  %10d", s.lCounts[2]);
	Diagnostics->Add(L"   Ignore :  %10d", s.lCounts[3]);
	Diagnostics->Add(L"   Client :  %10d", s.lCounts[4]);

	Diagnostics->Add(L"");
	Diagnostics->Add(L"Running Threads");
	for(auto i : RunningThreads)
	{
		Diagnostics->Add(L"  " + i->Name);;
	}

	Diagnostics->Add(L"Pending Threads");
	for(auto i : PendingThreads)
	{
		Diagnostics->Add(L"  " + i->Name);;
	}

	Diagnostics->Add(L"");
	Diagnostics->Add(L"Jobs");
	{
		std::lock_guard<std::mutex> guard(JobsLock);
	
		for(auto i : Jobs)
		{
			Diagnostics->Add(L"  " + i->Name);;
		}
	}

	Diagnostics->Add(L"");

	for(auto i : PerformanceCounters)
	{
		Diagnostics->Add(L"%-30s   min: %10.f   max: %10.f   %d nps", i->Name, 1e6 * i->Min, 1e6 * i->Max, i->GetMeasures());
	}
}

CList<CUrq> CCore::FindStartCommands(CUsl & s)
{
	CList<CUrq>	r;

	for(auto i : Commands)
	{
		if((i.Domain.empty() || i.Domain == s.Domain) && i.GetSystem() == s.Server)
		{
			r.push_back(i);
		}
	}

	return r;
}

bool CCore::HasCommand(CString const & name, CString const & val)
{
	auto cmds = FindStartCommands(CUsl(L"", L"Core"));
	for(auto & i : cmds)
	{
		if(i.Query.Contains(name, val))
		{
			return true;
		}
	}

	return false;
}
