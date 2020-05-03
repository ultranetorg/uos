#include "StdAfx.h"
#include "WindowScreen.h"

using namespace uos;

CWindowScreen::CWindowScreen(CEngineLevel * l, DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) : 
									CNativeWindow(l->Core, dwExStyle, lpWindowName, dwStyle, x, y, w, h, parent, menu, instance, param), CScreen(l)
{
	RECT r = {x, y, x + w, y + h};
	AdjustWindowRectEx(&r, dwStyle, menu != null, dwExStyle);

	SetRect(CGdiRect(r));

	Rect.Set(x, y, w, h);
	NRectWork.Set(x, y, w, h);

	auto b = CreateSolidBrush(RGB(0, 0, 0));
	auto dc = GetDC(Hwnd);
	RECT rc = {0, 0, Rect.Width, Rect.Height};
	FillRect(dc, &rc, b);
	ReleaseDC(Hwnd, dc);
	DeleteObject(b);
}

CWindowScreen::~CWindowScreen()
{
}

bool CWindowScreen::ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r)
{
	MSG m = {hwnd, msg, w, l};

	MessageRecieved(m);

	switch(msg)
	{
		case WM_CLOSE:
		{	
			Core->Exit();
			*r = 0;
			return true;
		}	
		case WM_SIZE:
		{
			if(w == SIZE_MAXIMIZED)
			{
				*r = 0;
				return true;
			}
			if(w == SIZE_MINIMIZED)
			{
				*r = 0;
				return true;
			}
			break;
		}
		default:
			break;
	}
	return false;
}

void CWindowScreen::Show(bool e)
{
	CNativeWindow::Show(e);
}

void CWindowScreen::SetCursor(HCURSOR c)
{
	CNativeWindow::SetCursor(c);
}
