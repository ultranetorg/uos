#pragma once
#include "IType.h"
#include "IWriter.h"
#include "NativePath.h"
#include "Directory.h"

namespace uos
{
	struct CPoolFile
	{
		CString			SourcePath;
		CString			DestinationPath;
		bool			Added;
		
		CPoolFile()
		{
			Added = false;
		}
	};

	struct CRelocatedFile
	{
		CString			Name;
		CString			Path;
	};


	class UOS_LINKING CWritePool : public IType
	{
		public:
			void										Add(const CString & dst, const CString & src, IWriterProgress * p);
			void										RelocateSource(const CString & path);
			bool										IsRelocated(const CString & src);

			UOS_RTTI
			CWritePool(IDirectory * d);
			~CWritePool();

		private:
			CList<CPoolFile>							Files;
			CList<CRelocatedFile>						RelocatedFiles;
			IDirectory *								Directory;
	};
}