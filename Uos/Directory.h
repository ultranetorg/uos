#pragma once
#include "NexusObject.h"
#include "File.h"

namespace uos
{
	struct CStorageEntry
	{
		CString		Path;
		CString		Type;
		CString		NameOverride;

		CStorageEntry(){}
		CStorageEntry(CString const & p) : Path(p){}
		CStorageEntry(CString const & p, CString const & t) : Path(p), Type(t) {} 
	};

	struct CFSRegexItem
	{
		CString						Path;
		CString						Type;
		CArray<CString>				Matches;

		CFSRegexItem(){}
		CFSRegexItem(CString const & p) : Path(p){}
	};


	class IDirectory
	{
		public:
			virtual CList<CStorageEntry> 				Enumerate(CString const & mask) = 0;
			virtual CList<CFSRegexItem>					EnumerateByRegex(CString const & pattern)=0;
			virtual CStream *							OpenWriteStream(CString const & name) = 0;
			virtual CStream * 							OpenReadStream(CString const & name) = 0;
			virtual void								Close(CStream * s) = 0;
			virtual void								Delete()=0;

			virtual ~IDirectory(){}
	};

	class UOS_LINKING CDirectory : public CNexusObject, public IDirectory
	{
		public:
			UOS_RTTI
			CDirectory(CServer * s, CString const & name) : CNexusObject(s, name) {}
	};

	class UOS_LINKING CDirectoryServer : public CDirectory
	{
		public:
			CString										Path;

			UOS_RTTI
			CDirectoryServer(CServer * s, CString const & path, CString const & npath);
			~CDirectoryServer();

			CList<CStorageEntry>						Enumerate(CString const & mask) override;
			CList<CFSRegexItem>							EnumerateByRegex(CString const & pattern);
			virtual CStream *							OpenWriteStream(CString const & name) override;
			virtual CStream * 							OpenReadStream(CString const & name) override;
			virtual void								Close(CStream * s) override;
			void virtual								Delete() override;
	};
}
