#pragma once
#include "Stream.h"

namespace uos
{
	struct ISerializable
	{
		virtual std::wstring							GetTypeName()=0;

		virtual void									Read(CStream * s)=0;
		virtual int64_t									Write(CStream * s)=0;

		virtual void									Write(std::wstring &)=0;
		virtual void									Read(const std::wstring & b)=0;

		virtual ISerializable *							Clone()=0;

		virtual bool									Equals(const ISerializable & a) const=0;

		int64_t WriteSize(CStream * s, int64_t size)
		{
			if(size <= 63)
			{
				size = size << 2;
				return s->Write(&size, 1);
			}
			else if(size <= (1 << 14) - 1)
			{
				size = size << 2 | 0b01;
				return s->Write(&size, 2);
			}
			else if(size <= (1 << 30) - 1)
			{
				size = size << 2 | 0b10;
				return s->Write(&size, 4);
			}
			else if(size <= (1ull << 62) - 1)
			{
				size = size << 2 | 0b11;
				return s->Write(&size, 8);
			}

			throw CException(HERE, L"Larger than 64bit not supported");
		}

		int64_t ReadSize(CStream * s)
		{
			int64_t size = 0;
			s->Read(&size, 1);

			if((size & 0b11) == 0b00)
			{
				//return size >> 2;
			}
			else if((size & 0b11) == 0b01)
			{
				s->Read(((unsigned char *)&size) + 1, 1);
			}
			else if((size & 0b11) == 0b10)
			{
				s->Read(((unsigned char *)&size) + 1, 1);
				s->Read(((unsigned char *)&size) + 2, 1);
				s->Read(((unsigned char *)&size) + 3, 1);
			}
			else if((size & 0b11) == 0b11)
			{
				s->Read(((unsigned char *)&size) + 1, 1);
				s->Read(((unsigned char *)&size) + 2, 1);
				s->Read(((unsigned char *)&size) + 3, 1);
				s->Read(((unsigned char *)&size) + 4, 1);
				s->Read(((unsigned char *)&size) + 5, 1);
				s->Read(((unsigned char *)&size) + 6, 1);
				s->Read(((unsigned char *)&size) + 7, 1);
			}

			return size >> 2;
		}

		virtual ~ISerializable(){}
	};
}