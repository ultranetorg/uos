#pragma once
#include "Security.h"
#include "OsNT.h"
#include "Supervisor.h"
#include "PerformanceCounter.h"
#include "CpuMonitor.h"
#include "LicenseService.h"
#include "Thread.h"
#include "XonDocument.h"
#include "NativeDirectory.h"
#include "ILevel.h"
#include "Sha256.h"
#include "Shared.h"

namespace uos
{
	#undef AddJob

	class CCore;
	class CMmc;

	enum class ESystemPath
	{
		Root, Servers, ServersCommon, Common
	};

	struct CLevel1
	{
		CLog *		Log;
		CCore *		Core;
	};

	struct CUosNodeInformation
	{
		DWORD	ProcessId = 0;
		DWORD	MainThreadId = 0;
		HWND	Mmc = 0;
	};

	struct CJob
	{
		IType *					Owner;
		CString					Name;
		std::function<bool()>	Work;
	};

	struct IWorker
	{
		virtual bool HasInstantWork()=0;
		virtual void DoInstant()=0;
	};

	struct IIdleWorker
	{
		virtual void DoIdle()=0;
	};

	class CExecutionParameters : public virtual CShared
	{
		public:
			virtual ~CExecutionParameters(){}
	};

	struct ICommandExecutor
	{
		virtual void Execute(const CUrq & u, CExecutionParameters * ep)=0;
	};

	UOS_LINKING wchar_t * 					GetSupervisorDataFolder();

	class UOS_LINKING CCore : public IType, public CLevel1, public ILevel
	{
		public:
			CString										Unid;

			CEvent<>										SecTick;
			CEvent<>										Suspended;
			CEvent<>										Resumed;
			CEvent<>										MmcActivated;
			CEvent<>										ExitQueried;

			CList<CUrq>									Commands;
			CString										SupervisorName;
			CString										RestartCommand;
			
			int											LastCommit = -1;
			CList<CString>								Commits;
			bool										CommitDatabase = true;
			CString										DatabaseObject;
			
			CString										CorePath;
			CString										RootPath;
			CString										CommonPath;
			CString										ServersPath;
			CString										UserPath;

			CString										SourceDirectory;
			CString										DestinationDirectory;
			CString										WorkDirectory;
			CString										WorkTmpFolder;

			CUosNodeInformation	*						Information = null;
			HANDLE										HInformation;
			CString										LaunchFolder;
			CString										FrameworkDirectory;
			CString										LaunchPath;
			HANDLE										SingleInstanceHandle = null;
			bool										Initialized = false;
			bool										DatabaseInitialized = false;

			HINSTANCE									LocationInstance;
			HINSTANCE									CreationInstance;
			CProductInfo								Product;

			CXonDocument *								DBConfig;
			CXonDocument *								Config = null;
			CXonDocument *								DefaultConfig = null;

			int											MsgWaitDelay;
			int											IdleCps;

			CString										FolderAppDataProduct;

			COsNT *										Os = null;
			CSupervisor *								Supervisor = null;
			ILicenseService *							LicenseService = null;
			CMmc *										Mmc = null;

			bool										Exiting = false;
			bool										SuspendStatus = false;
			bool										NeedErrorReporting = false;
			bool										IsAdministrating = false;
			CLevel1Timings								Timings;

			CList<ICommandExecutor *>					Executors;
			CList<CJob *>								Jobs;
			std::mutex									JobsLock;
			CList<IWorker *>							Workers;
			CList<IIdleWorker *>						IdleWorkers;
			CArray<HANDLE>								Events;
			CArray<std::function<void()>>				EventHandlers;
			CList<INativeMessageHandler *>				NmHandlers;
			CArray<std::function<void(uint64_t)>>		HotKeys;
			CArray<CPerformanceCounter *>				PerformanceCounters;
			CList<CThread *>							RunningThreads;
			CList<CThread *>							PendingThreads;

			CDiagnostic *								Diagnostics = null;
			CTimer										Timer;
			CTimer										TickTimer;
			CTimer										SecTimer;
			CPerformanceCounter *						PCCycle;

			UOS_RTTI
			CCore(CSupervisor * s, HINSTANCE i, wchar_t ** argv, int argc, const wchar_t * supervisor_folder, const wchar_t * root_from_exe, const wchar_t * coredir, CProductInfo & pi);
			~CCore();

			void										InitializeUnid();
			void										InitializeDatabase();
			void										ShutdownDatabase();

			void										AddRestartCommand(CUrl const & cmd);
			CList<CUrq>									FindStartCommands(CUsl & s);
			bool										HasCommand(CString const & name, CString const & val);
			void										SetCommit(bool c);
			CString										ResolveConstants(CString const & dir);
			CString 									MapToDatabase(const CString & path);
			CString										MapToTmp(const CString & path);

			void										Run();
			void										Exit();
			void										Suspend();
			void										Resume();

			void 										ProcessEvents(int i);
			bool 										ProcessMessages(MSG & m);
			void										ProcessIdle();
			void										ProcessCopyData(COPYDATASTRUCT * s);
			CString									 	GetPathTo(ESystemPath folder, const CString & path);


			void										AddPerformanceCounter(IPerformanceCounter *);
			void										RemovePerformanceCounter(IPerformanceCounter * name);

			void										AddJob(IType * owner, const CString & name, std::function<bool()> j);
			void										CancelJobs(IType * owner);
			CThread	*									RunThread(const CString & name, std::function<void()> main, std::function<void()> exit);

			void 										RegisterExecutor(ICommandExecutor * e);
			void										AddWorker(IWorker *);
			void										RemoveWorker(IWorker *);
			void										AddWorker(IIdleWorker *);
			void										RemoveWorker(IIdleWorker *);
			void										RegisterNativeMessageHandler(INativeMessageHandler *);
			void										UnregisterNativeMessageHandler(INativeMessageHandler *);
			void										RegisterEvent(HANDLE e, std::function<void()> fired);
			void										UnregisterEvent(HANDLE e);
			int											RegisterGlobalHotKey(int modifier, int vk, std::function<void(int64_t)> h);
			void										UnregisterGlobalHotKey(int id);
			

			void										Execute(const CUrq & c, CExecutionParameters * p);

			void										LoadParameters();
			void 										OnDiagnosticsUpdate(CDiagnosticUpdate & a);


	};
}