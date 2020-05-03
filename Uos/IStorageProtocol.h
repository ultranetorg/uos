#pragma once
#include "File.h"
#include "Directory.h"
#include "Path.h"

namespace uos
{
	#define UOS_FILE_SYSTEM_PROTOCOL					L"Uos.Storage"
	#define UOS_FILE_SYSTEM								L"Uos.Storage"
	#define UOS_FILE_SYSTEM_ORIGIN						L"una.ultranet.org/Uos.Storage"

	class IStorageProtocol : public virtual IProtocol
	{
		public:
			virtual CStream *							OpenWriteStream(CString const & path)=0;
			virtual CStream *							OpenReadStream(CString const & path)=0;
			virtual CObject<CDirectory>					OpenDirectory(CString const & path)=0;
			virtual void								Close(CStream *)=0;
			virtual void								Close(CDirectory *)=0;
			virtual void								DeleteFile(CString const & path)=0;
			virtual void								DeleteDirectory(CString const & path)=0;

			virtual CString								UniversalToNative(CString const & name)=0;
			virtual CString								NativeToUniversal(CString const & path)=0;
			
			virtual										~IStorageProtocol(){}
	};
}


