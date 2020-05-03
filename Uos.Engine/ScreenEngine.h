#pragma once
#include "WindowScreen.h"
#include "DirectSystem.h"

namespace uos
{
	class UOS_ENGINE_LINKING CScreenEngine : public CEngineEntity
	{
		public:
			CDiagnostic *								Diagnostics;
			CArray<CScreen *>							Screens;
			CScreen *									PrimaryScreen;
			CDirectSystem *								DisplaySystem;

			CEvent<CScreen *>							ScreenAdded;

			CList<CString>								Layouts;

			void										OnDiagnosticsUpdate(CDiagnosticUpdate & a);
						
			void										Update();

			CScreen *									GetPrimaryScreen();
			CScreen *									GetScreenByName(const CString & name);
			CScreen *									GetScreenByRect(CGdiRect & r);
			CScreen *									GetScreenByDisplayDeviceName(const CString & n);
			CScreen *									GetScreenByWindow(HWND hwnd);

			void										TakeScreenshot(CString const & name);

			void										SetLayout(const CString & layoutName);

			UOS_RTTI
			CScreenEngine(CEngineLevel * l, CDirectSystem * ge);
			~CScreenEngine();

		private:
			void										SetLayout(CArray<CXon *> & ls, const CString & layoutName);
			void										CreateScreen(CXon * p, CDisplayDevice * dd);

			void										OnLevel1Suspended();
			void										OnLevel1Resumed();
			void										OnLevel1ExitQueried();

			CXon *										Parameter;
			IPerformanceCounter *						PcUpdate;
	};
}