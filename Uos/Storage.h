#pragma once
#include "Server.h"
#include "IStorageProtocol.h"
#include "AsyncFileStream.h"

namespace uos
{
	class UOS_LINKING CStorage : public CServer, public IStorageProtocol, protected CDirectoryServer
	{	
		public:
			CLevel2 *									Level;
			CMap<CString, CString>						Mounts;

			CList<CStream *>							WriteStreams;
			CList<CStream *>							ReadStreams;
			CList<CDirectoryServer *>					Directories;

			UOS_RTTI
			CStorage(CLevel2 * l, CServerInfo * info);
			~CStorage();

			void										Start(EStartMode sm) override;
			IProtocol * 								Connect(CString const & pr) override;
			void										Disconnect(IProtocol * c) override;

			void										CreateDirectory(CString const & path);
			void										CreateAllDirectories(CString const & path);
			void										CreateMounts(CServerInfo * s);

			CStream *									OpenWriteStream(CString const & path) override;
			CStream *									OpenReadStream(CString const & path) override;
			CAsyncFileStream *							OpenAsyncReadStream(CString const & path);
			CObject<CDirectory>							OpenDirectory(CString const & path) override;
			void										Close(CDirectory *) override;
			void										Close(CStream *) override;
			
			void										DeleteFile(CString const & path) override;
			void										DeleteDirectory(CString const & path) override;

			CString										UniversalToNative(CString const & path) override;
			CString										NativeToUniversal(CString const & path) override;
			CString										Resolve(CString const & u);

			CString										MapPath(CString const & mount, CString const & path);
			CString 									MapPath(CUsl & u, CString const & path);

			//void										CreateGlobalDirectory(CString const & path);
			void										CreateGlobalDirectory(CNexusObject * o, CString const & path = CString());
			void										CreateGlobalDirectory(CServer * s, CString const & path = CString());
			void										CreateLocalDirectory(CString const & path);
			void										CreateLocalDirectory(CNexusObject * o, CString const & path = CString());
			void										CreateLocalDirectory(CServer * s, CString const & path = CString());

			bool										Exists(CString const & name);


			CList<CStorageEntry>						Enumerate(CString const & dir, CString const & mask);
			CList<CStorageEntry>						Enumerate(CString const & mask);

			CUol										ToUol(CString const & type, CString const & path);
			CString										GetType(CString const & path);

			bool										CanRemap(CString const & path);
			CString										RemapToCommon(CString const & path);

	};
}
