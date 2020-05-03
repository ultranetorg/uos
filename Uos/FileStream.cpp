#include "StdAfx.h"
#include "FileStream.h"

using namespace uos;


CFileStream::CFileStream(const CString & path, EFileMode mode)
{
/*
	if(mode == EFileMode::Open && !CPath::IsExists(path))
	{
		throw CException(HERE, L"File not found: %s", path.c_str());
	}
*/
	Path = path;
	Mode = mode;
	
	std::ios::openmode m=0;
	switch(mode)
	{
		case EFileMode::Open :
			m = std::ios::in | std::ios::binary;
			break;
		case EFileMode::New :
			m = std::ios::out | std::ios::binary;
			break;
	}
	if(m != 0)
	{
		Open(m);
	}
}

CFileStream::~CFileStream()
{
	if(Stream.is_open())
	{
		Stream.close();
	}
}

void CFileStream::Open(std::ios::openmode mode)
{
	auto p = CNativePath::IsUNCServer(Path) ? Path : (L"\\\\?\\" + Path);
	Stream.open(p.c_str(), mode);

	if(Stream.fail())
	{
		throw CFileException(HERE, CString::Format(L"File opening failed: %s", Path));
	}
}

bool CFileStream::IsValid()
{
	return !Stream.fail();
}

int64_t CFileStream::GetSize()
{
	auto p = Stream.tellg();

	Stream.seekg(0, std::ios::end);
	auto size = Stream.tellg();
	Stream.seekg(p, std::ios::beg);
	return size;
}

int64_t CFileStream::Read(void * p, int64_t size)
{
	Stream.read((char *)p, size);
	return (int)Stream.gcount();
}

int64_t CFileStream::Write(const void * p, int64_t size)
{
	if(size > 0)
	{
		if(Mode == EFileMode::NewIfNeeded)
		{
			Open(std::ios::out|std::ios::binary);
		}
		Stream.write((const char *)p, size);

		return size;
	}
	else
		return 0;
}

int64_t CFileStream::GetPosition()
{
	return Stream.tellg();
}

void CFileStream::ReadSeek(int64_t n)
{
	Stream.seekg(n, std::ios::cur);
}

void CFileStream::WriteSeek(int64_t n)
{
	Stream.seekp(n, std::ios::cur);
}

bool uos::CFileStream::IsEnd()
{
	return Stream.eof();
}

