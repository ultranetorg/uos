#pragma once
#include "Level2.h"
#include "NexusObject.h"

namespace uos
{
	struct CServerInfo
	{
		CUrl											Origin;
		CString											Role;
		bool											Installed = false;
		CUsl 											Url;
		CString											InstallPath;
		CString											ObjectsPath;
		HINSTANCE										HInstance;
		CXon *											Xon;
		CVersion										Version;
		CXonDocument *									Registry = null;

		~CServerInfo()
		{
			delete Registry;
		}
	};

	enum class EStartMode
	{
		Installing, Start
	};

	class UOS_LINKING CServer : public virtual IType
	{
		public:
			CLevel2 *									Level;
			CUsl										Url;
			CServerInfo *								Info;
			//CString									GlobalDataPath;

			CList<CNexusObject *>						Objects;
			
			CMap<CString, IProtocol *>					Protocols;
			CMap<CString, CList<IType *>>				Users;
			CEvent<CServer *, IProtocol *, CString &>	Disconnecting;
			
			UOS_RTTI
			CServer(CLevel2 * l, CServerInfo * info);
			~CServer();

			virtual void								Execute(const CUrl & u, CExecutionParameters * ep){}
			virtual void								Start(EStartMode sm){}
			
			virtual IProtocol *		 					Connect(CString const & pr)=0;
			virtual void								Disconnect(IProtocol * s)=0;

			virtual CNexusObject *						CreateObject(CString const & name);
			void										RegisterObject(CNexusObject * o, bool shared);
			void										DeleteObject(CNexusObject * r);
			bool										Exists(CString const & name);
			void										LoadObject(CNexusObject * o);
			void										DestroyObject(CNexusObject * o, bool save = true);

			CNexusObject *								FindObject(CString const & name);
			CNexusObject *								FindObject(CUol const & u);
			template<class T> T *						FindObject(CString const & name)
														{
															return FindObject(name)->As<T>();
														}

			CTonDocument *								LoadServerDocument(CString const & path);
			CTonDocument *								LoadGlobalDocument(CString const & path);


			CString										MapRelative(CString const & path);
			CString										MapUserLocalPath(CString const & path);
			CString										MapUserGlobalPath(CString const & path);
			CString										MapTmpPath(CString const & path);
			CString										MapPath(CString const & path);
	};

	typedef CServer *	(* FStartUosHub)(CLevel2 * l, CServerInfo * info);
	typedef void	(* FStopUosHub)();
}

