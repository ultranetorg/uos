#include "StdAfx.h"
#include "KdCursorModificator.h"

#if 0

namespace mw
{
	SC_HANDLE	CKdCursorModificator::SCManager;

	CKdCursorModificator::CKdCursorModificator(CGodLevel * ew) : CEntity3God(ew)
	{
		CVersion v = Level1->GetOs()->GetVersion();

		if(v.Era != 5 || v.Release != 1)
		{
			throw CAttentionException(HERE, L"Windows XP 32bit supported only");
		}
			
		IsInstalled		= false;
		IsCursorHidden	= false;
		DriverFilePath	= Level2->GetPathToModule(Level3->GetModuleDescriptor(), L"Mightywill.Engine.CursorModifier-x86-5.1.xxxx.sys");
		File			= CString(L"\\\\.\\") + DRIVER_NAME;
				
		FileHandle= null;
		SCManager = OpenSCManager(null, null, SC_MANAGER_ALL_ACCESS);
		
		ReportMessage(L"win32k.sys: version=%s", CVersion::GetFromFile(L"win32k.sys").ToString().c_str());

		Install();
		if(IsInstalled)
		{
			int status = QueryBypassStatus();
			
			switch(status)
			{
				case CURSORMODIFIER_STATUS_POSSIBLE:
					ReportMessage(L"Initial status: Bypass possible");
					HideCursor();
					break;

				case CURSORMODIFIER_STATUS_IMPOSSIBLE:
					wchar_t s[1024];
					GetSystemDirectory(s, _countof(s));
					wcscat_s(s, L"\\win32k.sys");
					CopyFile(s, SuperVisor->GetPathToDataFolder(L"win32k.sys").c_str(), false);
					throw CException(HERE, L"Initial status: Bypass impossible");
					break;

				case CURSORMODIFIER_STATUS_ALREADYSET:
					ReportMessage(L"Initial status: Bypass is already set");
					IsCursorHidden = true;
					break;

				default:
					throw CException(HERE, L"Initial status: unknown");
					break;
			}
			Level1->Suspended	+= EventHandler(CKdCursorModificator::OnSuspendingStarted);
			Level1->Resumed		+= EventHandler(CKdCursorModificator::OnResumingStarted);
		}
	}
	
	CKdCursorModificator::~CKdCursorModificator()
	{
		if(IsInstalled)
		{
			Level1->Suspended	-= EventHandler(CKdCursorModificator::OnSuspendingStarted);
			Level1->Resumed		-= EventHandler(CKdCursorModificator::OnResumingStarted);

			if(IsCursorHidden)
			{
				ShowCursor();
			}
			Uninstall(false);
		}

		CloseServiceHandle(SCManager);

//		RegDeleteKey(HKEY_LOCAL_MACHINE, (CString(L"SYSTEM\\CurrentControlSet\\Services\\") + DRIVER_NAME + CString(L"\\Enum")).c_str());
//		RegDeleteKey(HKEY_LOCAL_MACHINE, (CString(L"SYSTEM\\CurrentControlSet\\Services\\") + DRIVER_NAME + CString(L"\\Security")).c_str());
//		RegDeleteKey(HKEY_LOCAL_MACHINE, (CString(L"SYSTEM\\CurrentControlSet\\Services\\") + DRIVER_NAME).c_str());
	}

	void CKdCursorModificator::OnSuspendingStarted()
	{
		ShowCursor();
	}

	void CKdCursorModificator::OnResumingStarted()
	{
		HideCursor();
	}
		
	void CKdCursorModificator::Install()
	{
		SERVICE_STATUS ss;
		Service = OpenService(SCManager, DRIVER_NAME, SERVICE_ALL_ACCESS);
		
		if(Service == null)
		{	
			Service = CreateService(SCManager, DRIVER_NAME, DRIVER_NAME, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,	SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,	DriverFilePath.c_str(), null, null, null, null, null);
		}
		
		if(Service == null)
		{
			throw CLastErrorException(HERE, GetLastError(), L"Couldn`t create. ");
			return;
		}
		
		QueryServiceStatus(Service, &ss);
		if(ss.dwCurrentState != SERVICE_RUNNING)
		{
			if(!StartService(Service, 0, null))
			{
				DWORD d = GetLastError(); 
				if(d != 1056) // An instance of the service is already running. 
				{
					Uninstall(true);
					throw CLastErrorException(HERE, d, L"Couldn`t start. ");
				}
			}
		}
		
		FileHandle = CreateFile(File.c_str(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);// 2) Теперь блокируем возможность изменения
		if(FileHandle == INVALID_HANDLE_VALUE)
		{
			Uninstall(true);
			throw CLastErrorException(HERE, GetLastError(), L"Couldn`t create file: %s  ", File.c_str());
		}
		
		IsInstalled = true;
		ReportMessage(L"KdCursorModificator installed");
	}

	void CKdCursorModificator::Uninstall(bool fully)
	{
		bool success = true;
		SERVICE_STATUS	ss;

		CloseHandle(FileHandle);

		if(fully)
		{
			if(!ControlService(Service, SERVICE_CONTROL_STOP, &ss))
			{
				success = false;
			}
			if(!DeleteService(Service))
			{
				success = false;
			}
		}
		
		if(!CloseServiceHandle(Service))
		{
			success = false;
		}

		if(success)
		{
			ReportMessage(L"KdCursorModificator successfully uninstalled");
		}
	}
	
	void CKdCursorModificator::HideCursor() 
	{
		if(!IsCursorHidden)
		{
			DWORD n;
			
			POINT p;
			GetCursorPos(&p);
			SetForegroundWindow(Level1->GetMmcWindow()->GetHwnd());
			RECT r;
			GetWindowRect(Level1->GetMmcWindow()->GetHwnd(), &r);
			SetCursorPos(r.left + 10, r.top + 10);
	
			ClipCursor(&r);
			HCURSOR cr = LoadCursor(Level3->GetModuleDescriptor()->GetInstance(), MAKEINTRESOURCE(IDC_CURSOR_FAKE));
			SetCursor(cr);
			ClipCursor(null);
	
			
		//	2) Теперь блокируем возможность изменения
			DeviceIoControl(FileHandle, IOCTL_CURSORMODIFIER_SET_BYPASS, null, 0, null, 0, &n, null);// байпасим!
		}
		
		IsCursorHidden = true;
	}

	void CKdCursorModificator::ShowCursor()
	{ 
		if(IsCursorHidden)
		{
			DWORD n;
			DeviceIoControl(FileHandle, IOCTL_CURSORMODIFIER_REMOVE_BYPASS, null, 0, null, 0, &n, null);// 2) Снимаем шунт в ядре  
		}

		SetForegroundWindow(Level1->GetMmcWindow()->GetHwnd());
		RECT r;
		GetWindowRect(Level1->GetMmcWindow()->GetHwnd(), &r);
		SetCursorPos(r.left + 10, r.top + 10);

		ClipCursor(&r);
		SetCursor(LoadCursor(null, IDC_ARROW));
		ClipCursor(null);
		
		SystemParametersInfo(SPI_SETCURSORS, 0, 0, SPIF_SENDCHANGE);

		IsCursorHidden = false;
	}

	int CKdCursorModificator::QueryBypassStatus() 
	{
		DWORD	bytesReturn;
		DWORD	ret = 0;

		if(!DeviceIoControl(FileHandle, IOCTL_CURSORMODIFIER_QUERY_STATUS, null, 0, &ret, 4, &bytesReturn, null))
		{
			throw CLastErrorException(HERE, GetLastError(), L"DeviceIoControl failed. ");
		}
		return ret;
	}
}

#endif