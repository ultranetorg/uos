#include "stdafx.h"
#include "File.h"

using namespace uos;
using namespace std::experimental::filesystem;

CLocalFile::CLocalFile(CServer * s, CString const & path, CString const & name) : CFile(s, name)
{
	Path = path;
}

CLocalFile::~CLocalFile()
{
	for(auto i : Streams)
	{
		delete i;
	}
}

void CLocalFile::Delete(CString const & path)
{
	DeleteFile((L"\\\\?\\" + path).data());;
}

bool CLocalFile::Exists(CString const & file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file.data(), &FindFileData) ;
	int found = handle != INVALID_HANDLE_VALUE;
	if(found) 
	{
		FindClose(handle);
	}
	return found;
}
