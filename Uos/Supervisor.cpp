#include "StdAfx.h"
#include "SuperVisor.h"

using namespace uos;

CSupervisor::CSupervisor()
{
}

CSupervisor::~CSupervisor()
{
	for(auto i : Logs)
	{
		delete i;
	}
	for(auto i : Diagnosticss)
	{
		delete i;
	}
}

void CSupervisor::StartWriting(const CString & directory)
{
	Directory = directory;

	for(auto i : Logs)
	{
		i->StartWriting(directory);
	}
}

void CSupervisor::StopWriting()
{
	for(auto i : Logs)
	{
		i->StopWriting();
	}
}

void CSupervisor::Commit()
{
	for(auto i : Logs)
	{
		i->Commit();
	}
}

CLog * CSupervisor::CreateLog(const CString & name)
{
	CLog * l = new CLog(this, name);

	if(MainLog)
	{
		l->MainLog = MainLog;
	}

	if(!Directory.empty())
	{
		l->StartWriting(Directory);
	}

	Logs.push_back(l);
	LogCreated(l);
	return l;
}

CDiagnostic * CSupervisor::CreateDiagnostics(const CString & name)
{
	CDiagnostic * d = new CDiagnostic(name);
	Diagnosticss.push_back(d);
	DiagnosticsCreated(d);
	return d;
}

CLog * CSupervisor::GetLog(const CString & name)
{
	for(auto i : Logs)
	{
		if(i->GetName() == name)
		{
			return dynamic_cast<CLog *>(i);
		}
	}
	return null;
}

CDiagnostic * CSupervisor::GetDiagnostics(const CString & name)
{
	for(auto i : Diagnosticss)
	{
		if(i->GetName() == name)
		{
			return dynamic_cast<CDiagnostic *>(i);
		}
	}
	return null;
}

void CSupervisor::WriteDiagnosticFile(const CString & name, CString const & content)
{
	DWORD n;
	HANDLE file	= CreateFile(CNativePath::Join(Directory, name).c_str(), GENERIC_WRITE, FILE_SHARE_READ, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, null);
	WriteFile(file, "\xFF\xFE", 2, &n, null);
	
	if(file != INVALID_HANDLE_VALUE)
	{
		WriteFile(file, content.c_str(), (DWORD)content.size()*2, &n, null);			
		CloseHandle(file);
	}
}

CString CSupervisor::GetPathToDataFolder(const CString & path)
{
	return CNativePath::Join(Directory, path);
}
