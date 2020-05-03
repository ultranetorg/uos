#pragma once
#include "Buffer.h"
#include "Exception.h"

namespace uos
{
	#undef CStream

	enum class EFileMode
	{
		New, Open, NewIfNeeded
	};

	class CStream
	{
		public:
			virtual int64_t	Read(void * p, int64_t size)=0;
			virtual int64_t	Write(const void * p, int64_t size)=0;

			virtual int64_t GetSize()
			{
				throw CException(HERE, L"Not supported");
			}

			virtual bool IsEnd()
			{
				throw CException(HERE, L"Not supported");
			}
			
			virtual int64_t GetPosition()
			{
				throw CException(HERE, L"Not supported");
			}

			virtual void WriteSeek(int64_t n) // move forward
			{
				throw CException(HERE, L"Not supported");
			}
			virtual void ReadSeek(int64_t n) // move forward
			{
				throw CException(HERE, L"Not supported");
			}
			
			virtual void Reset()
			{
				ReadSeek(-GetPosition());
			}

			CBuffer Read()
			{
				return Read(GetSize());
			}

			CBuffer Read(int64_t size)
			{
				CBuffer b(size);
				Read(b.GetData(), b.GetSize());
				return b;
			}
				
			virtual int64_t Write(CStream * src)
			{
				CBuffer b = src->Read();
				return Write(b.GetData(), b.GetSize());
			}
	
			virtual int64_t Write(CBuffer & b)
			{
				return Write(b.GetData(), b.GetSize());
			}

			
			virtual ~CStream(){}
	};

}
