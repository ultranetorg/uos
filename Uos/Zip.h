#pragma once
#include "Exception.h"
#include "Directory.h"
#include "ZipStream.h"
#include "NativePath.h"
#include "Array.h"
#include "Storage.h"

namespace uos
{
	class UOS_LINKING CZipDirectory : public IDirectory
	{
		public:
			virtual CStream *							OpenWriteStream(CString const & name) override;
			virtual CStream * 							OpenReadStream(CString const & name) override;
			virtual void								Close(CStream * s) override;
			virtual CList<CStorageEntry>						Enumerate(CString const & mask) override;
			virtual CList<CFSRegexItem>					EnumerateByRegex(CString const & mask) override;
			virtual void								Delete();

			void										Add(const CString & dst, CStream * src);
			void										Add(const CString & dst, CStream * src, IWriterProgress * p);
			bool										Contains(const CString & filepath);

			int											GetSize(const CString & entrypath);
			CString										GetUri(const CString & entrypath);
			
			CString										GetUri();
			EFileMode									GetMode();
			void *										GetHandle();

			CZipDirectory(const CString & zippath, EFileMode mode);
			CZipDirectory(CStream * s, EFileMode mode);
			~CZipDirectory();


		private:
			CString										ZipPath;	
			void *										ZFile;
			EFileMode									Mode;
			CBuffer										Buffer;

	};


	class UOS_LINKING CZip
	{
		public:
			static std::vector<uint8_t>					Compress(void *in_data, size_t in_data_size);
			static CArray<char>							Decompress(void *in_data, size_t in_data_size);
	};
}
