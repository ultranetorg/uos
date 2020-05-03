#include "StdAfx.h"
#include "Buffer.h"

using namespace uos;

CBuffer::CBuffer()
{
}

CBuffer::CBuffer(const CBuffer & b) : CBuffer(((CBuffer &)b).GetData(), b.Size)
{

}

CBuffer::CBuffer(int64_t size) : CBuffer(null, size)
{
}

CBuffer::CBuffer(const void * data, int64_t size)
{
	Set(data, size);
}

CBuffer::CBuffer(CBuffer && b)
{
	Data = b.Data;
	Size = b.Size;

	b.Size = 0;
}

CBuffer::~CBuffer()
{
	if(Size > sizeof(Data))
	{
		//void * d;
		//CopyMemory(&d, Data, sizeof(d));
		free((void *)Data);
	} 
}

void CBuffer::Set(const void * data, int64_t size)
{
	if(Size != size && Size > sizeof(Data))
	{
		//void * d;
		//CopyMemory(&d, Data, sizeof(d));
		//free(d);
		free((void *)Data);
	} 


	if(size > 0)
	{
		if(size <= sizeof(Data))
		{
			if(data != null)
			{
				CopyMemory(&Data, data, (size_t)size);
			}
		}
		else
		{
			Data = (uint64_t)malloc((size_t)size);
			//CopyMemory(Data, &d, sizeof(d));

			if(data != null)
			{
				CopyMemory((void *)Data, data, (size_t)size);
			}
		}
	}

	Size = size;
}

void CBuffer::Add(const void * data, int64_t size)
{
	CBuffer b = *this;
	Set(null, b.GetSize() + size);
	CopyMemory(GetData(),						b.GetData(), (size_t)b.GetSize());
	CopyMemory((char *)GetData() + b.GetSize(), data,		 (size_t)size);
}

void CBuffer::Add(CBuffer & b)					
{
	Add(b.GetData(), b.Size);
}

CBuffer & CBuffer::operator = (const CBuffer & b)
{
	Set(((CBuffer &)b).GetData(), b.Size);
	return *this;
}

void * CBuffer::GetData()
{
	if(Size <= sizeof(Data))
	{
		return (void *)&Data;
	} 
	else
	{
		return (void *)Data;
	}
}

int64_t CBuffer::GetSize() const
{
	return Size;
}


bool CBuffer::operator != (const CBuffer & aa)
{
	auto a = const_cast<CBuffer &>(aa);
	return GetSize() != a.GetSize() || memcmp(GetData(), a.GetData(), (size_t)GetSize()) != 0;
}


