#include "StdAfx.h"
#include "MemoryStream.h"

using namespace uos;

CMemoryStream::CMemoryStream()
{
}

CMemoryStream::~CMemoryStream()
{
}

bool CMemoryStream::IsValid()
{
	return !Stream.fail();
}

int64_t CMemoryStream::GetSize()
{
	auto p = Stream.tellp();
	Stream.seekp(0, std::ios::end);
	auto s = (int64_t)Stream.tellp();
	Stream.seekp(p);
	return s;
}

int64_t CMemoryStream::Read(void * p, int64_t size)
{
	Stream.read((char *)p, size);
	return Stream.gcount();
}

int64_t CMemoryStream::Write(const void * p, int64_t size)
{
	if(size > 0)
	{
		Stream.write((const char *)p, size);
	}
	return size;
}

int64_t CMemoryStream::GetPosition()
{
	return Stream.tellg();
}

void CMemoryStream::ReadSeek(int64_t n)
{
	Stream.seekg(n);
}

void CMemoryStream::WriteSeek(int64_t n)
{
	Stream.seekp(n);
}

bool CMemoryStream::IsEnd()
{
	return Stream.eof();
}

void CMemoryStream::Clear()
{
	Stream.clear();
	Stream.str("");
}

