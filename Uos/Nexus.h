#pragma once
#include "Connection.h"
#include "Storage.h"
#include "Base58.h"
#include "IExecutorProtocol.h"
#include "ILevel.h"
#include "Ethereum.h"
#include "Dms.h"

namespace uos
{
	class UOS_LINKING CNexus : public ILevel, public IType, public ICommandExecutor, public IWorker
	{
		public:
			CLevel2										Level2;
			CCore *										Core;
			CStorage *									Storage;

			int											ExitHotKeyId;
			int											SuspendHotKeyId;
			
			CList<CServerInfo *>						Infos;
			CList<CServer *>							Servers;
			
			CXonDocument *								Config = null;
			CXonDocument *								DefaultConfig = null;
			CDiagnostic *								Diagnostic;

			CString										DirectoryPath;
			CString										ObjectTemplatePath;
			
			CEvent<>										Initialized;
			CEvent<>										Stopping;
			
			CString										RestartCommand;
			
			CDms *										Dms;
			CFdn *										Fdn;

			CString										UpdateStatus;
			CList<CProductRelease *>					NewReleases;
			CEvent<>										UpdateStatusChanged;

			UOS_RTTI
			CNexus(CCore * l, CXonDocument * config);
			~CNexus();

			void										Restart(CString const & cmd);

			void										ProcessHotKey(int64_t id);
			void										OnDiagnosticUpdating(CDiagnosticUpdate & a);
			void										StartServers();
			void										StopServers();
			void										StopServer(CServer * s);

			void										Execute(const CUrq & o, CExecutionParameters * p);

			CMap<CServerInfo *, CXon *>					GetRegistry(CString const & path);
			CXon *										GetRegistry(CUsl & s, CString const & path);

			void										SetDllDirectories();
			CString										GetExecutable(CString const & spath);

			CConnection 								Connect(IType * who, CUsl & o, CString const & p);
			CConnection 								Connect(IType * who, CString const & p);
			CList<CConnection> 							ConnectMany(IType * who, CString const & p);
			void										Disconnect(CConnection & c);
			void										Disconnect(CList<CConnection> & c);

			//CSystem *									GetSystem(CUsl & u);
			CServer *									GetServer(CString const & name);

			CList<CUsl>									FindImplementators(CString const & pr);

			void										Break(CUsl & u, CString const & pr);
		
			
			virtual bool HasInstantWork()				override;
			virtual void DoInstant()					override;

			template<class T> CProtocolConnection<T>	Connect(IType * who, CUsl & u, CString const & p)
														{
															auto c = Connect(who, u, p);

															if(c && c.As<T>() == null)
															{
																return CProtocolConnection<T>();
															}
															return CProtocolConnection<T>(c);
														}

			template<class T> CProtocolConnection<T>	Connect(IType * who, CString const & p)
														{
															return CProtocolConnection<T>(Connect(who, p));
														}

			template<class T> 
			CList<CProtocolConnection<T>>				ConnectMany(IType * who, CString const & p)
														{
															CList<CProtocolConnection<T>> cc;
															for(auto c : ConnectMany(who, p))
															{
																cc.push_back(CProtocolConnection<T>(c));
															}
															return cc;
														}

			template<class T> void						Disconnect(CList<CProtocolConnection<T>> & cc)
														{
															for(auto & c : cc)
															{
																Disconnect(c);
															}
														}
	};
}