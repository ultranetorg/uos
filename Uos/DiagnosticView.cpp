#include "StdAfx.h"
#include "DiagnosticView.h"

using namespace uos;

CDiagnosticView::CDiagnosticView(CCore * e, CDiagnostic * diag, CGdiRect & r, CNativeWindow * mmc, int menuItemID) : 
					CNativeWindow(e, 0, L"", WS_VISIBLE|WS_SYSMENU|WS_THICKFRAME, (int)r.X, (int)r.Y, (int)r.Width, (int)r.Height, mmc->Hwnd, null, e->Core->LocationInstance, null)
{
	Core		= e;
	Mmc			= mmc;
	MenuItemID	= menuItemID;
	Diag		= diag;
	Core->SecTick += ThisHandler(Update);
	Diag->Added += ThisHandler(OnAdded);
	Diag->Appended += ThisHandler(OnAppended);

	Init();
	SetTitle(diag->GetName()+L" - Diagnostics");
	Show(true);
}

void CDiagnosticView::Init()
{
	DC			= GetDC(Hwnd);

	MemBitmap	= null;
	MemDC		= CreateCompatibleDC(DC);
	BkBrush		= CreateSolidBrush(RGB(0,0,0));

	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	CString face = L"Lucida Console";
	lf.lfHeight = -10;
	wcscpy_s(lf.lfFaceName, face.size()+1, face.c_str());
	Font = CreateFontIndirect(&lf);
	FontOld = (HFONT)SelectObject(MemDC, (HGDIOBJ)Font); 

	SetTextColor(MemDC, RGB(0, 204, 0));
	SetBkMode(MemDC, TRANSPARENT);

	SIZE s;
	GetTextExtentPoint32(MemDC, L"W", 1, &s);
	CharWidth	= s.cx;
	CharHeight	= s.cy;

	ResizeBitmaps();

}

CDiagnosticView::~CDiagnosticView()
{
	Core->SecTick -= ThisHandler(Update);
	Diag->Added -= ThisHandler(OnAdded);
	Diag->Appended -= ThisHandler(OnAppended);

	SelectObject(MemDC, FontOld);
	DeleteObject(Font);
	SelectObject(MemDC, MemBitmapOld);
	DeleteObject(MemBitmap);
	DeleteDC(MemDC);
	ReleaseDC(Hwnd, DC);
	DeleteObject(BkBrush);
}

CDiagnostic * CDiagnosticView::GetDiagnosticsToShow()
{
	return Diag;
}	

bool CDiagnosticView::ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r)
{
	PAINTSTRUCT ps;
	HDC dc;

	switch(msg)
	{
		case WM_PAINT:
			dc = BeginPaint(hwnd, &ps);
			BitBlt(dc, 0, 0, (int)RectClient.GetWidth(), (int)RectClient.GetHeight(), MemDC, 0, 0, SRCCOPY);
			EndPaint(hwnd, &ps);
			*r = 1;
			return true;

		case WM_ERASEBKGND:
			*r = 1;
			return true;

		case WM_MOUSEMOVE:
			Highlighted.x = LOWORD(l);
			Highlighted.y = HIWORD(l);
			Update();
			return false;

		case WM_MOUSEWHEEL:
			UpdateData.Offset += GET_WHEEL_DELTA_WPARAM(w)/WHEEL_DELTA * -6;
			UpdateData.Offset = max(UpdateData.Offset, 0);
			UpdateData.Count = RectClient.Height/CharHeight + 1;
			Update();
			return false;

		case WM_KEYDOWN:
			return false;
			
		case WM_SIZE:
			ResizeBitmaps();
			break;

		case WM_CLOSE:
			*r = 0;
			PostMessage(Mmc->Hwnd, WM_COMMAND, MAKEWPARAM(MenuItemID, 0), 0);
			return true;
	}
	return false;
}

void CDiagnosticView::Update()
{
	CTimer t(true);

	FillRect(MemDC, &RectClient.GetAsRECT(), BkBrush);
	SetBkMode(MemDC, OPAQUE);
			
	Diag->Clear();
	Diag->Updating(UpdateData);
	
	Present();

	SetTitle(Diag->GetName() + L" - Diagnostics - " + CString::Format(L"%g", t.GetTime()));
}

void CDiagnosticView::ResizeBitmaps()
{
	RectClient = GetClientRect();

	if(MemBitmap != null)
	{
		SelectObject(MemDC, MemBitmapOld);
		DeleteObject(MemBitmap);
	}

	MemBitmap = CreateCompatibleBitmap(DC, (int)RectClient.Width, (int)RectClient.Height);
	MemBitmapOld = (HBITMAP)SelectObject(MemDC, MemBitmap);

	UpdateData.Offset = 0;
	UpdateData.Count = RectClient.Height/CharHeight + 1;

	Present();
}

void CDiagnosticView::OnAdded(CString & t, DWORD c)
{
	assert(t.size() < INT_MAX);

	X = 0;

	if(UpdateData.Offset <= Diag->Lines && Diag->Lines < UpdateData.Offset + UpdateData.Count)
	{
		auto y = (Diag->Lines - UpdateData.Offset - 1) * CharHeight;
	
		if(y <= Highlighted.y && Highlighted.y < y + CharHeight && Highlighted.x > 0 && Highlighted.x < RectClient.Width)
		{
			SetBkColor(MemDC, RGB(64, 64, 64));
		}
		else
		{
			SetBkColor(MemDC, RGB(0, 0, 0));
		}
		
		if(!t.empty())
		{
			SetTextColor(MemDC, c);
			TextOut(MemDC, X, y, t.c_str(), (int)t.size());
		}
		SIZE s;
		GetTextExtentPoint32(MemDC, t.c_str(), (int)t.size(), &s);
		X += s.cx;
	}
}

void CDiagnosticView::OnAppended(CString & t, DWORD c)
{
	assert(t.size() < INT_MAX);

	auto y = (Diag->Lines - UpdateData.Offset - 1) * CharHeight;
		
	SetTextColor(MemDC, c);
	TextOut(MemDC, X, y, t.c_str(), (int)t.size());
	SIZE s;
	GetTextExtentPoint32(MemDC, t.c_str(), (int)t.size(), &s);
	X += s.cx;
}

void CDiagnosticView::Present()
{
	//InvalidateRect(Hwnd, NULL, false);
	BitBlt(DC, 0, 0, (int)RectClient.GetWidth(), (int)RectClient.GetHeight(), MemDC, 0, 0, SRCCOPY);
}
