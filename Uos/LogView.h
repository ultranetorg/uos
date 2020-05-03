#pragma once
#include "NativeWindow.h"
#include "Log.h"
#include "Core.h"

namespace uos
{
	class CLogView : public CNativeWindow
	{
		public:
			UOS_RTTI
			CLogView(CCore * lv, CLog * l, CGdiRect & r, CNativeWindow * mmc, int menuItemID);
			CLogView(CCore * lv, CLog * l, HWND hwnd);
			~CLogView();

			CLog *										GetLogToShow();
			void										ShowLog(CLog * l);
			void										ShowEnable(bool e);
			
		private:
			CLog *										Log;
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
			
			HFONT										FontOld;

			void										Init();
			void										OnMessageReceived(CLog * log, CLogMessage & m);
			bool										ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r);
			void										FullUpdate();
			void										DrawLine(const wchar_t * m, ELogSeverity rs, int y);
			void										ResizeBitmaps();
			void										OnPaint(HDC dc);
	};
}
