#pragma once
#include "LogView.h"
#include "DiagnosticView.h"
#include "Resources\resource.h"
//#include "Config.h"
#include "Core.h"
#include "ILevel.h"

namespace uos
{
	struct CMmcMenuItem
	{
		int					Id;
		CLog *				Log = null;
		CDiagnostic *		Diag = null;
		CNativeWindow *		View = null;
		void *				Object = null;
		CXon *				Parameter = null;
	};

	class CNexus;

	class UOS_LINKING CMmc : public CNativeWindow
	{
		public:
			void										Activate();
			void										Deactivate();
			void										EnableLevelControl(bool e);

			UOS_RTTI
			CMmc(CCore * e, CXonDocument * config);
			~CMmc();
			
		private:
			CCore *										Core;
			CNexus *									Nexus;
			CString										StartPosition;
			CString										DeactivationAction;
			CXonDocument *								Config;
			CXon *										Parameter;

			CNativeWindow *								View;
			HMENU										MainMenu;
			HMENU										FileMenu;
			HMENU										HelpMenu;
			HMENU										LogsMenu;
			HMENU										DiagsMenu;
			HMENU										ObjectsMenu;
			
			CGdiRect									DefaultPosition;
			
			CList<CMmcMenuItem>							Items;

			int NextId()
			{
				int i=1000;
				for(; i<65535; i++)
				{
					if(!Items.Has( [i](auto & j){ return i == j.Id; } ))
					{
						return i;
					}
				}
				throw CException(HERE, L"Next Menu Id not found");
			}
			//int											LogIdEnd;
			//
			//int											DiagIdStart;
			//int											DiagIdEnd;
			//
			//int											ObjectIdStart;
			//int											ObjectIdEnd;
			
			void										OnLevelCreated(int n, ILevel * l);
			bool										ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r);
			void										OnLogCreated(CLog * l);
			void										OnDiagnosticsCreated(CDiagnostic * d);
			void										OnSize();
			void										InitMenus();
			void										DestroyMenus();
			void 										AddLog(CLog * log);
			void 										AddDiag(CDiagnostic * diag);
			
			//void										OnViewClosed(CNativeWindow *&, UINT &, WPARAM w);
			void										ShowLog(CMmcMenuItem * i, bool show);
			void										ShowDiag(CMmcMenuItem * i, bool show);

			void										OnExitQueried();
			void										OnLevel1Suspended();
			void										OnLevel1Resumed();
	};
}
