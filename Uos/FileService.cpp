#include "StdAfx.h"
#include "FileService.h"

using namespace uos;

CFileService::CFileService()
{
}

CFileService::~CFileService()
{
}

void CFileService::WriteUTF16(const CString & path, const CString & text)
{
	DWORD n;
	HANDLE file	= CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, null);
	
	if(file != INVALID_HANDLE_VALUE)
	{
		WriteFile(file, "\xFF\xFE", 2, &n, null);
		WriteFile(file, text.c_str(), (DWORD)text.size()*2, &n, null);			
		CloseHandle(file);
	}
}

void CFileService::WriteANSI(const CString & path, const CAnsiString & text)
{
	DWORD n;
	HANDLE file	= CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, null);

	if(file != INVALID_HANDLE_VALUE)
	{
		WriteFile(file, text.c_str(), (DWORD)text.size(), &n, null);
		CloseHandle(file);
	}
}
