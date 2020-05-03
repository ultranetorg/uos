#pragma once
#include "NativeWindow.h"
#include "Diagnostic.h"
#include "Core.h"

namespace uos
{
	class CDiagnosticView : public CNativeWindow
	{
		public:
			CDiagnostic *								GetDiagnosticsToShow();

			UOS_RTTI
			CDiagnosticView(CCore * e, CDiagnostic * d, CGdiRect & r, CNativeWindow * mmc, int menuId);
			~CDiagnosticView();

			void OnAdded(CString & t, DWORD c);
			void OnAppended(CString & t, DWORD c);
		private:
			CCore *										Core;
			CDiagnostic *								Diag;
			CNativeWindow *								Mmc;
			int											MenuItemID;

			HDC											DC;
			HDC											MemDC;
			HBITMAP										MemBitmap;
			HBITMAP										MemBitmapOld;
			HBRUSH										BkBrush;
			HFONT										Font;
			CGdiRect									RectClient;
			int											CharHeight;
			int											CharWidth;
			POINT										Highlighted;

			HFONT										FontOld;
			CDiagnosticUpdate							UpdateData;

			int											X;

			void										Update();
			bool										ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r);
			void										ResizeBitmaps();
			void										Present();
			void										Init();
	};
}
