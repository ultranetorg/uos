#pragma once

#if 0

#include "EngineLevel.h"

#include <tlhelp32.h>
#include <winioctl.h>
#include <winsvc.h>

#include "..\Mightywill.Module\Mightywill.Module.Engine.CursorModifier\DriverGlobals.h"
#include "ILevel3God.h"

namespace mw
{
	class CKdCursorModificator : public CEntity3God
	{
		public:
			UOS_DEFINE_RTTI
			CKdCursorModificator(CGodLevel * ew);
			~CKdCursorModificator();
			
		private:
			static SC_HANDLE							SCManager;		// эта переменная должна быть глобальная 
			CString										DriverFilePath;
			CString										File;
			SC_HANDLE									Service;
			bool										IsInstalled;
			bool										IsCursorHidden;
			HANDLE										FileHandle;
			
			void										Install();
			void										Uninstall(bool fully);
			void										HideCursor();
			void										ShowCursor();
			int											QueryBypassStatus();
			void 										OnSuspendingStarted();
			void 										OnResumingStarted();
	};
}

#endif
