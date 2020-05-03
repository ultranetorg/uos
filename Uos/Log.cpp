#include "StdAfx.h"
#include "Log.h"
#include "Supervisor.h"

using namespace uos;

CString CLog::MessageFormat	= L"%6lld: %-25s: %-2s: %s";

CLog::CLog(CSupervisor * sv, const CString & name)
{
	Supervisor		= sv;
	Name			= name;
}

CLog::~CLog()
{
	if(LogFile)
	{
		StopWriting();
	}
}

void CLog::Commit()
{
	FlushFileBuffers(LogFile);
}
	
CString & CLog::GetName()
{
	return Name;
}

void CLog::StartWriting(const CString & directory)
{
	DWORD n;
	LogFile	= CreateFile(CNativePath::Join(directory, (Name + L".log")).c_str(), GENERIC_WRITE, FILE_SHARE_READ, null, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, null);

	WriteFile(LogFile, "\xFF\xFE", 2, &n, null);

	for(auto & i : Pending)
	{
		WriteFile(LogFile, i.Text.c_str(), (DWORD)i.Text.size() * 2, &n, null);
		WriteFile(LogFile, L"\r\n", 4, &n, null);
	}

	Pending.clear();
}

void CLog::StopWriting()
{
	Commit();
	CloseHandle(LogFile);
	LogFile = null;
}

void CLog::Report(ELogSeverity sev, IType * who, const wchar_t * str)
{
	DWORD n;
	wchar_t full[4*1024];
	wchar_t * ss = L"";

	switch(sev)
	{
		case ELogSeverity::Warning:
			ss = L"W";
			break;
		case ELogSeverity::Error:
			ss = L"E";
			break;
		case ELogSeverity::Debug:
			ss = L"D";
			break;
	}
	swprintf_s(full, _countof(full), MessageFormat.c_str(), Supervisor->Cycles, who->GetInstanceName().data(), ss, str);

	{
		std::lock_guard<std::mutex> guard(Lock);

		Messages.push_back(CLogMessage(full, sev));
		auto & u = Messages.back().Text;

		if(LogFile)
		{
			WriteFile(LogFile, u.c_str(), (DWORD)u.size()*2, &n, null);
			WriteFile(LogFile, L"\r\n", 4, &n, null);
		}
		else
		{
			Pending.push_back(Messages.back());
		}
		
		if(Messages.size() > 50)
		{	
			Messages.pop_front();
		}
		
		MessageReceived(this, Messages.back());
		
		if(MainLog)
		{
			if(sev == ELogSeverity::Error || sev == ELogSeverity::Warning)
			{
				MainLog->Report(sev, who, str);
			}
		}
	}
}
