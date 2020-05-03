#pragma once
#include "Stream.h"
#include "Exception.h"

namespace uos
{
	class UOS_LINKING CMemoryStream : public CStream
	{
		public:
			CMemoryStream();
			~CMemoryStream();

			using CStream::Write;
			using CStream::Read;

			bool										IsValid();
			int64_t										Read(void * p, int64_t size) override;
			int64_t										Write(const void * p, int64_t size) override;
			int64_t										GetSize();
			int64_t										GetPosition();
			void										ReadSeek(int64_t n);
			void										WriteSeek(int64_t n);
			virtual bool								IsEnd() override;
			void										Clear();

		private:
			EFileMode									Mode;
			std::stringstream							Stream;
	};
}
