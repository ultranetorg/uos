#pragma once
#include "GdiRect.h"
#include "Core.h"

namespace uos
{
	#define UOS_DEFAULT_WINDOW_CLASS						L"UosGeneralClass"
	#undef IsMinimized
	
	class UOS_LINKING CNativeWindow : public IType
	{
		public:
			HWND										Hwnd;
			bool virtual								ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r);

			HWND										GetHwnd();
			void										Show(bool e);
			void										SetSize(int w, int h);
			void										SetPosition(int x, int y);
			void										SetRect(CGdiRect & r);
			CGdiRect									GetClientRect();
			CGdiRect									GetRect();
			void										SetTitle(const CString & t);
			CString										GetTitle();
			bool										IsMinimized();
			bool										IsMaximazed();
			bool										IsVisible();
			void										SetCursor(HCURSOR c);
			void										BringToTop();

			CEvent<CNativeWindow *>						Closed;
			//static K2P1Event<CNativeWindow *, UINT, WPARAM>		GlobalMessageEvent;

			UOS_RTTI
			CNativeWindow(CCore * l, DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
			CNativeWindow(CCore * l, HINSTANCE i, int t, HWND parent);
			CNativeWindow(CCore * l, HWND handle);
			virtual ~CNativeWindow();


		protected:
			CCore *										Core;
			bool										IsCreated;
			ATOM static									DefaultClass;
			CMap<HWND, CNativeWindow *>	static			Windows;
			CMap<HWND, CNativeWindow *>	static			Forms;
			LRESULT static								WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			INT_PTR static								FormProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			void										RegisterDefaultClass();
			HCURSOR										Cursor = null;
	};
}