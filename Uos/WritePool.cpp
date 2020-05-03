#include "StdAfx.h"
#include "WritePool.h"
#include "FileStream.h"

using namespace uos;

CWritePool::CWritePool(IDirectory * d)
{
	Directory = d;
}

CWritePool::~CWritePool()
{
}

void CWritePool::RelocateSource(const CString & path)
{
	for(auto & i : RelocatedFiles)
	{
		if(i.Path == path)
		{
			return;
		}
	}

	CRelocatedFile f;
	f.Path = path;
	f.Name = CNativePath::GetFileName(path);
	RelocatedFiles.push_back(f);
}

void CWritePool::Add(const CString & dst, const CString & src, IWriterProgress * p)
{
	CString name = CNativePath::GetFileName(src);
	
	CString srcReal = src;

	for(auto & i : RelocatedFiles)
	{
		if(i.Name == name)
		{
			srcReal = i.Path;
			break;
		}
	}

	for(auto & i : Files)
	{
		if(i.DestinationPath == dst && i.SourcePath == srcReal && i.Added)
		{
			return;
		}
	}
	
	CPoolFile pf;
	pf.Added			= true;
	pf.DestinationPath	= dst;
	pf.SourcePath		= srcReal;
	
	auto s = Directory->OpenWriteStream(dst);
	s->Write(&CFileStream(srcReal, EFileMode::Open));
	Directory->Close(s);
	
	Files.push_back(pf);
}

bool CWritePool::IsRelocated(const CString & src)
{
	CString name = CNativePath::GetFileName(src);
	
	for(auto & i : RelocatedFiles)
	{
		if(i.Name == name)
		{
			return true;
		}
	}
	return false;
}
