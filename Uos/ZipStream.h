#pragma once
#include "Exception.h"
#include "IWriterProgress.h"
#include "NativePath.h"
#include "MemoryStream.h"

namespace uos
{
	class CZipDirectory;

	class UOS_LINKING CZipStream : public CStream, public IType
	{
		public:
			int64_t										Read(void * data, int64_t size);
			int64_t										Write(const void * data, int64_t size);
			int64_t 									Write(const void * data, int64_t size, IWriterProgress * p);
			int64_t										Write(CStream * inStream, IWriterProgress * p);
			int64_t										Write(CStream * inStream);
			void										Seek(int64_t size);
			int64_t										GetSize();
			CString										GetUri();
			
			virtual bool								IsEnd() override;

			UOS_RTTI
			CZipStream(CZipDirectory * zip, const CString & entrypath);
			~CZipStream();


		private:
			CString										EntryPath;
			CZipDirectory *								Zip;
			CMemoryStream 								UnzStream;
	};
}
