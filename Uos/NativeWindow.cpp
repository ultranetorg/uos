#include "StdAfx.h"
#include "NativeWindow.h"

using namespace uos;

CMap<HWND, CNativeWindow *>						CNativeWindow::Windows;
CMap<HWND, CNativeWindow *>						CNativeWindow::Forms;
ATOM											CNativeWindow::DefaultClass = null;
//K2P1Event<CNativeWindow *, UINT, WPARAM>		CNativeWindow::GlobalMessageEvent;

CNativeWindow::CNativeWindow(CCore * l, DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	Core = l;

	RegisterDefaultClass();

	IsCreated = true;

	Hwnd = CreateWindowEx(dwExStyle, UOS_DEFAULT_WINDOW_CLASS, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if(Hwnd == null)
	{
		throw CLastErrorException(HERE, GetLastError(), L"CreateWindowEx failed");
	}
	Windows[Hwnd] = this;
}

CNativeWindow::CNativeWindow(CCore * l, HINSTANCE i, int t, HWND parent)
{
	Core = l;

	RegisterDefaultClass();

	IsCreated = true;
		
	Hwnd = CreateDialog(i, MAKEINTRESOURCE(t), parent, (DLGPROC)FormProc);
	if(Hwnd == null)
	{
		throw CLastErrorException(HERE, GetLastError(), L"CreateDialog failed");
	}
	Forms[Hwnd] = this;
}

CNativeWindow::CNativeWindow(CCore * l, HWND handle)
{
	Core = l;

	RegisterDefaultClass();

	IsCreated	= false;
	Hwnd		= handle;
	Windows[Hwnd] = this;
}
	
CNativeWindow::~CNativeWindow()
{
	if(Hwnd != null)
	{
		Windows.erase(Hwnd);
		Forms.erase(Hwnd);	

		if(IsCreated)
		{
			DestroyWindow(Hwnd);
		}
	}
}
	
void CNativeWindow::Show(bool e)
{
	ShowWindow(Hwnd, e ? SW_SHOW : SW_HIDE);
	UpdateWindow(Hwnd);
}
	
void CNativeWindow::RegisterDefaultClass()
{
	if(!DefaultClass)
	{
		HICON	icon16;
		HICON	icon32;
		UINT	iconId;
		PrivateExtractIcons(Core->LaunchPath.data(), 0, 16, 16, &icon16, &iconId, 1, LR_DEFAULTCOLOR);
		PrivateExtractIcons(Core->LaunchPath.data(), 0, 32, 32, &icon32, &iconId, 1, LR_DEFAULTCOLOR);

		HMODULE wndprocModule;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPWSTR)WindowProc, &wndprocModule);
	
		WNDCLASSEX wcex;
		wcex.cbSize			= sizeof(WNDCLASSEX); 
		wcex.style			= CS_DBLCLKS;
		wcex.lpfnWndProc	= (WNDPROC)WindowProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= wndprocModule;
		wcex.hIcon			= icon32;
		wcex.hCursor		= LoadCursor(null, IDC_ARROW);
		wcex.hbrBackground	= null;
		wcex.lpszMenuName	= null;
		wcex.lpszClassName	= UOS_DEFAULT_WINDOW_CLASS;
		wcex.hIconSm		= icon16;
		DefaultClass = RegisterClassEx(&wcex);
	}
}

INT_PTR CNativeWindow::FormProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	auto  i = Forms.find(hwnd);

	if(i != Forms.end())
	{
		INT_PTR r;
		if(i->second->ProcessMessage(hwnd, msg, w, l, &r)) // don`t need default procession
		{
			return TRUE;
		}
		else
		{
			switch(msg)
			{
				case WM_CLOSE:
					i->second->Closed(i->second);
					break;
				default:
					break;
			}
		}
		//GlobalMessageEvent(i->second, msg, w);
	}
	return FALSE;
}

LRESULT CNativeWindow::WindowProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	auto i = Windows.find(hwnd);

	if(i != Windows.end())
	{
		INT_PTR r;

		if(i->second->ProcessMessage(hwnd, msg, w, l, &r)) // don`t need default procession
		{
			return r;
		}
		else
		{
			switch(msg)
			{
				case WM_CLOSE:
					i->second->Closed(i->second);
					break;

				case WM_SETCURSOR:
					if(i->second->Cursor)
					{
						::SetCursor(i->second->Cursor);
						return TRUE;
					}
					break;
			}
		}
	}
	return DefWindowProc(hwnd, msg, w, l);
}
	
bool CNativeWindow::ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r)
{
	return false;
}
	
HWND CNativeWindow::GetHwnd()
{
	return Hwnd;
}

CString CNativeWindow::GetTitle()
{
	wchar_t t[256];
	InternalGetWindowText(Hwnd, t, _countof(t));
	return t;
}

void CNativeWindow::SetTitle(const CString & t)
{
	SetWindowText(Hwnd, t.c_str());
}

void CNativeWindow::SetRect(CGdiRect & r)
{	
	SetWindowPos(Hwnd, Hwnd, r.X, r.Y, r.Width, r.Height, SWP_NOZORDER);
}

void CNativeWindow::SetPosition(int x, int y)
{	
	CGdiRect r = GetRect();
	MoveWindow(Hwnd, x, y, r.Width, r.Height, false);
}

void CNativeWindow::SetSize(int w, int h)
{	
	CGdiRect r = GetRect();
	MoveWindow(Hwnd, r.X, r.Y, w, h, false);
}

CGdiRect CNativeWindow::GetRect()
{	
	RECT r;
	GetWindowRect(Hwnd, &r);
	return r;
}

CGdiRect CNativeWindow::GetClientRect()
{	
	RECT r;
	::GetClientRect(Hwnd, &r);
	return r;
}

bool CNativeWindow::IsMinimized()
{	
	return IsIconic(Hwnd)==TRUE;
}

bool CNativeWindow::IsMaximazed()
{	
	return IsZoomed(Hwnd)==TRUE;
}
	
void CNativeWindow::SetCursor(HCURSOR c)
{
	Cursor = c;

	::SetCursor(c);
	//Level->Log->ReportDebug(this, L"d", c);
	//PostMessage(Hwnd, WM_SETCURSOR, 0, 0);
	//SetClassLong(Hwnd, GCL_HCURSOR, (DWORD)c);
}

bool CNativeWindow::IsVisible()
{
	return IsWindowVisible(Hwnd) != 0;
}

void CNativeWindow::BringToTop()
{
	HWND hCurWnd = ::GetForegroundWindow();
	DWORD dwMyID = ::GetCurrentThreadId();
	DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
	::AttachThreadInput(dwCurID, dwMyID, TRUE);
	::SetWindowPos(Hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetWindowPos(Hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetForegroundWindow(Hwnd);
	::AttachThreadInput(dwCurID, dwMyID, FALSE);
	::SetFocus(Hwnd);
	::SetActiveWindow(Hwnd);

	//BringWindowToTop(Hwnd);
}

