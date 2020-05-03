#pragma once
#include "Stream.h"
#include "Exception.h"
#include "NativePath.h"
#include "IType.h"

namespace uos
{
	class CFileException : public CException
	{
		public:
			CFileException(wchar_t const * m, int l, std::wstring const & msg) : CException(m, l, msg)
			{
			}
	};

	class UOS_LINKING CFileStream : public CStream, public IType
	{
		public:
			CString										Path;
			EFileMode									Mode;
			std::fstream								Stream;

			UOS_RTTI
			CFileStream(const CString & filepath, EFileMode mode);
			~CFileStream();

			using CStream::Write;
			using CStream::Read;

			bool										IsValid();
			int64_t										Read(void * p, int64_t size) override;
			int64_t										Write(const void * p, int64_t size) override;
			int64_t										GetSize();
			int64_t										GetPosition();
			void										ReadSeek(int64_t n);
			void										WriteSeek(int64_t n);

			bool										IsEnd() override;

			void										Open(std::ios::openmode mode);
	};
}

