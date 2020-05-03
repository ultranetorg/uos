#include "StdAfx.h"
#include "LogView.h"

using namespace uos;

CLogView::CLogView(CCore * c, CLog * l, CGdiRect & r, CNativeWindow * mmc, int menuItemID) : CNativeWindow(c, 0, L"", WS_VISIBLE|WS_SYSMENU|WS_THICKFRAME, r.X, r.Y, r.Width, r.Height, mmc->Hwnd, null, c->LocationInstance, null)
{
	Mmc			= mmc;
	MenuItemID	= menuItemID;
	Log = null;
	Init();

	SetTitle(l->GetName()+L" - Log");
	ShowLog(l);
	Show(true);
}

CLogView::CLogView(CCore * lv, CLog * l, HWND hwnd) : CNativeWindow(lv, hwnd)
{
	Mmc			= null;
	MenuItemID	= 0;
	Log			= null;
	Init();

	SetTitle(l->GetName()+L" - Log");
	ShowLog(l);
	Show(true);
}

CLogView::~CLogView()
{
	if(Log)
	{
		Log->MessageReceived -= ThisHandler(OnMessageReceived);
	}

	SelectObject(MemDC, FontOld);
	DeleteObject(Font);
	SelectObject(MemDC, MemBitmapOld);
	DeleteObject(MemBitmap);
	DeleteDC(MemDC);
	ReleaseDC(Hwnd, DC);
	DeleteObject(BkBrush);
}

void CLogView::Init()
{
	DC			= GetDC(Hwnd);

	auto screen = GetDC(NULL);
	auto dpiy = GetDeviceCaps(screen, LOGPIXELSY);


	MemBitmap	= null;
	MemDC		= CreateCompatibleDC(DC);
	BkBrush		= CreateSolidBrush(RGB(0,0,0));

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	CString face = L"Lucida Console";
	lf.lfHeight = int(10.f * dpiy / 96);
	wcscpy_s(lf.lfFaceName, face.size()+1, face.c_str());
	Font = CreateFontIndirect(&lf);
	FontOld = (HFONT)SelectObject(MemDC, (HGDIOBJ)Font); 

	SetBkMode(MemDC, TRANSPARENT);

	SIZE s;
	GetTextExtentPoint32(MemDC, L"W", 1, &s);
	CharWidth	= s.cx;
	CharHeight	= s.cy;

	ResizeBitmaps();

}

void CLogView::ShowEnable(bool e)
{
	ShowWindow(Hwnd, e ? SW_SHOW : SW_HIDE);
}

void CLogView::ShowLog(CLog * l)
{
	if(Log != null)
	{
		Log->MessageReceived -= ThisHandler(OnMessageReceived);
	}

	Log = l;
	
	if(Log != null)
	{
		Log->MessageReceived += ThisHandler(OnMessageReceived);
		FullUpdate();
	}
}

CLog * CLogView::GetLogToShow()
{
	return Log;
}	

bool CLogView::ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r)
{
	PAINTSTRUCT ps;
	HDC dc;

	switch(msg)
	{
		case WM_PAINT:
			dc = BeginPaint(hwnd, &ps);
			OnPaint(dc);
			EndPaint(hwnd, &ps);
			*r = 1;
			return true;
			
		case WM_ERASEBKGND:
			*r = 1;
			return true;
			
		case WM_SIZE:
			ResizeBitmaps();
			break;

		case WM_CLOSE:
			if(MenuItemID != 0)
			{
				*r = 0;
				PostMessage(Mmc->Hwnd, WM_COMMAND, MAKEWPARAM(MenuItemID, 0), 0);
				return true;
			}
			break;
	}
	return false;
}

void CLogView::OnPaint(HDC dc)
{
	BitBlt(dc, 0, 0, RectClient.GetWidth(), RectClient.GetHeight(), MemDC, 0, 0, SRCCOPY);
}

void CLogView::ResizeBitmaps()
{
	RectClient = GetClientRect();

	if(MemBitmap != null)
	{
		SelectObject(MemDC, MemBitmapOld);
		DeleteObject(MemBitmap);
	}

	MemBitmap = CreateCompatibleBitmap(DC, RectClient.GetWidth(), RectClient.GetHeight());
	MemBitmapOld = (HBITMAP)SelectObject(MemDC, MemBitmap);
	FullUpdate();
}

void CLogView::OnMessageReceived(CLog * log, CLogMessage & m)
{
	BitBlt(MemDC, 0, 0, RectClient.GetWidth(), RectClient.GetHeight()-CharHeight, MemDC, 0, CharHeight, SRCCOPY);
	FillRect(MemDC, &CGdiRect(0, RectClient.GetHeight() - CharHeight, RectClient.GetWidth(), CharHeight).GetAsRECT(), BkBrush);
	DrawLine((wchar_t *)m.Text.c_str(), m.ReportStatus, RectClient.GetHeight() - CharHeight);
	BitBlt(DC, 0, 0, RectClient.GetWidth(), RectClient.GetHeight(), MemDC, 0, 0, SRCCOPY);
}

void CLogView::FullUpdate()
{
	FillRect(MemDC, &RectClient.GetAsRECT(), BkBrush);
	if(Log != null)
	{
		int y = RectClient.GetHeight() - CharHeight;

		for(auto i=Log->Messages.rbegin(); i!=Log->Messages.rend(); i++)
		{
			DrawLine(i->Text.c_str(), i->ReportStatus, y);
			y -= CharHeight;
			if(y < 0)
			{
				break;
			}
		}
	}
	BitBlt(DC, 0, 0, RectClient.GetWidth(), RectClient.GetHeight(), MemDC, 0, 0, SRCCOPY);
}

void CLogView::DrawLine(const wchar_t * m, ELogSeverity rs, int y)
{
	SetTextColor(MemDC, RGB(224,224,0));		TextOut(MemDC, 0,			y, m,	7);
	SetTextColor(MemDC, RGB(0,224,224));		TextOut(MemDC, CharWidth*7, y, m+7, 27);

	DWORD color = RGB(0, 224, 0);
	switch(rs)
	{
		case ELogSeverity::Warning:
			color = RGB(255, 0, 255);
			break;
		case ELogSeverity::Debug:
			color = RGB(255, 128, 0);
			break;
		case ELogSeverity::Error:
			color = RGB(255, 0, 0);
			break;
	}		

	SetTextColor(MemDC, color);
	TextOut(MemDC, CharWidth*34, y, m+34, (int)wcslen(m+34));
}
