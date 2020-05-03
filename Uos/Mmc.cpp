#include "StdAfx.h"
#include "Mmc.h"
#include "Nexus.h"

using namespace uos;

CMmc::CMmc(CCore * c, CXonDocument * config) : CNativeWindow(c, c->LocationInstance, IDF_MMC, null)
{
	Core		= c;
	Config		= config;
	Parameter	= Config->One(L"Mmc");

	Core->Information->Mmc = Hwnd;

	if(Parameter->Get<CString>(L"State") == L"Visible")
	{
		View = new CLogView(Core, Core->Log, GetDlgItem(GetHwnd(), IDC_MMCVIEW));
	
		DeactivationAction	= Parameter->One(L"DeactivationAction")->AsString();
			
		if(Parameter->One(L"StartSize")->Get<CString>() == L"Default")
		{
			SetSize(640, 320);
		}
		else if(Parameter->One(L"StartSize")->Get<CString>() == L"Custom")
		{
			CGdiRect r = Parameter->One(L"CustomRect")->Get<CGdiRect>();
			SetSize(r.Width, r.Height);
		}
	
		if(Parameter->One(L"StartPosition")->Get<CString>() == L"Default")
		{
			RECT pd;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &pd, 0);
			CGdiRect d = pd;
			CGdiRect r = GetRect();
			SetPosition((d.Width-r.Width)/2, (d.Height-r.Height)/2);
		}
		else if(Parameter->One(L"StartPosition")->Get<CString>() == L"Custom")
		{
			CGdiRect r = Parameter->One(L"CustomRect")->Get<CGdiRect>();
			SetPosition(r.X, r.Y);
		}
	
		DefaultPosition = GetRect();
	
		Show(true);
		SetTitle(Core->Product.ToString(L"NVSPB"));
	
		HRSRC s = FindResource(Core->LocationInstance, MAKEINTRESOURCE(IDR_FONT), L"TTF"/*, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)*/);
		void * rp = LockResource(LoadResource(Core->LocationInstance, s));
		DWORD n;
		AddFontMemResourceEx(rp, SizeofResource(Core->LocationInstance, s), 0, &n);
	
		Core->LevelCreated		+= ThisHandler(OnLevelCreated);
		Core->ExitQueried		+= ThisHandler(OnExitQueried);
		Core->Suspended			+= ThisHandler(OnLevel1Suspended);
		Core->Resumed			+= ThisHandler(OnLevel1Resumed);
	
		if(Core->IsAdministrating)
		{
			Core->Supervisor->LogCreated			+= ThisHandler(OnLogCreated);
			Core->Supervisor->DiagnosticsCreated	+= ThisHandler(OnDiagnosticsCreated);
		}
	
		InitMenus();
	}
}
	
CMmc::~CMmc()
{
	if(Parameter->Get<CString>(L"State") == L"Visible")
	{
		if(Core->IsAdministrating)
		{
			Core->Supervisor->LogCreated			-= ThisHandler(OnLogCreated);
			Core->Supervisor->DiagnosticsCreated	-= ThisHandler(OnDiagnosticsCreated);
		}

		Core->ExitQueried	-= ThisHandler(OnExitQueried);
		Core->LevelCreated	-= ThisHandler(OnLevelCreated);
		Core->Suspended		-= ThisHandler(OnLevel1Suspended);
		Core->Resumed		-= ThisHandler(OnLevel1Resumed);
	
		delete View;
			
		DestroyMenus();
		
		//for(auto i : Logs)
		//{
		//	if(i->View == null && i->Parameter != null)
		//	{
		//		Parameter->Remove(i->Parameter);
		//	}
		//	ShowLog(*i, false);
		//}
	
		for(auto & i : Items)
		{
			if(i.View == null && i.Parameter != null)
			{
				Parameter->Remove(i.Parameter);
			}
	
			if(i.Log)
				ShowLog(&i, false);
			if(i.Diag)
				ShowDiag(&i, false);
		}
			
		if(Parameter->One(L"SavePosition")->AsBool() && !IsMaximazed() && !IsMinimized())
		{
		}
	
		Parameter->One(L"CustomRect")->Set(GetRect());
	}
}

void CMmc::OnExitQueried()
{
	Activate();
}

void CMmc::OnLevelCreated(int n, ILevel * l)
{
	if(n == 1)
	{
		InitMenus();
	}
	if(n == 2)
	{
		Nexus = dynamic_cast<CNexus *>(l);
			
		if(!Core->IsAdministrating)
		{
			Deactivate();
		}
	}
}

void CMmc::OnLevel1Suspended()
{
	Activate();
	EnableMenuItem(FileMenu, ID_MENU_SUSPEND,	MF_BYCOMMAND|MF_GRAYED);
	EnableMenuItem(FileMenu, ID_MENU_RESUME,	MF_BYCOMMAND|MF_ENABLED);
}

void CMmc::OnLevel1Resumed()
{
	Deactivate();
	EnableMenuItem(FileMenu, ID_MENU_SUSPEND,	MF_BYCOMMAND|MF_ENABLED);
	EnableMenuItem(FileMenu, ID_MENU_RESUME,	MF_BYCOMMAND|MF_GRAYED);
}
	
void CMmc::Activate()
{
	if(!Core->IsAdministrating)
	{
		if(DeactivationAction == L"PutAway")
		{
			SetWindowPos(Hwnd, HWND_TOP, DefaultPosition.X, DefaultPosition.Y, 0, 0, SWP_NOSIZE);
		}
		else if(DeactivationAction == L"Hide")
		{
			ShowWindow(Hwnd, SW_SHOW);
		}
	}
	SetForegroundWindow(Hwnd);
}
		
void CMmc::Deactivate()
{
	if(!Core->IsAdministrating)
	{
		if(DeactivationAction == L"PutAway")
		{
			SetWindowPos(Hwnd, HWND_TOP, -32767, -32767, 0, 0, SWP_NOSIZE);
		}
		else if(DeactivationAction == L"Hide")
		{
			ShowWindow(Hwnd, SW_HIDE);
		}
	}
}
	
void CMmc::EnableLevelControl(bool e)
{
}

bool CMmc::ProcessMessage(HWND hwnd, UINT msg, WPARAM w, LPARAM l, INT_PTR * r)
{
	switch(msg) 
	{
		case WM_ACTIVATE:
			if(w == WA_ACTIVE || w == WA_CLICKACTIVE)
			{
				Core->MmcActivated();
			}
			break;
		
		case WM_INITDIALOG:
			break;

		case WM_CLOSE:
			Core->Exit();
			break;

		case WM_SIZE:
			OnSize();
			break;
				
		case WM_KEYDOWN:
			break;

		case WM_COPYDATA:
		{
			auto cd = (COPYDATASTRUCT *)l;
			Core->ProcessCopyData(cd);
			break;
		}
		case WM_INITMENU:
		{
			//
			DestroyMenu(ObjectsMenu);

			Items.remove_if([](auto & i){ return i.Object != null; });

			ObjectsMenu = CreateMenu();

			MENUITEMINFO mii;
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_SUBMENU;
			mii.hSubMenu = ObjectsMenu;

			SetMenuItemInfo(MainMenu, 3, true, &mii);

			for(auto s : Nexus->Servers)
				for(auto o : s->Objects)
				{
					Items.push_back(CMmcMenuItem());
					auto mi = &(Items.back());

					mi->Id			= NextId();
					mi->Object		= o;

					AppendMenu(ObjectsMenu, MF_STRING, Items.size(), o->Url.Object.c_str());

					MENUITEMINFO mii;
					mii.cbSize = sizeof(MENUITEMINFO);
					mii.fMask = MIIM_DATA;
					mii.dwItemData = (INT_PTR)mi;

					SetMenuItemInfo(ObjectsMenu, mi->Id, false, &mii);
				}


			break;
		}
		case WM_COMMAND:
		{
			int id = LOWORD(w);
			switch(id)
			{
				case ID_HELP_ABOUT:
					//Core->ShowAbout();
					break;

				case ID_HELP_REGISTRATION:
					//Core->ShowRegistration();
					break;

				case ID_FILE_EXIT:
					Core->Exit();
					break;

				case ID_MENU_SUSPEND:
					Core->Suspend();
					break;

				case ID_MENU_RESUME:
					Core->Resume();
					break;
					
				default:
					MENUITEMINFO mii;
					mii.cbSize = sizeof(MENUITEMINFO);
					mii.fMask = MIIM_DATA;
					
					auto mi = Items.Find([id](auto & j){ return j.Id == id; } );
					
					if(mi.Log)
					{
						GetMenuItemInfo(LogsMenu, id, false, &mii);
						auto mi = (CMmcMenuItem *)mii.dwItemData;
						ShowLog(mi, mi->View == null);
					}
					else if(mi.Diag)
					{
						GetMenuItemInfo(DiagsMenu, id, false, &mii);
						auto mi = (CMmcMenuItem *)mii.dwItemData;
						ShowDiag(mi, mi->View == null);
					}
					else if(mi.Object)
					{
						//Nexus->Destroy(((CObjectRelation *)mi.Object)->Object->Url);
					}
					break;
			}
			break;
		}				
	}
		
	return false;
}
	
void CMmc::OnSize()
{
	CGdiRect rr = GetClientRect();
	
	if(View != null)
	{
		View->SetRect(CGdiRect(0, 0, rr.Width, rr.Height));
	}
}
	
void CMmc::InitMenus()
{
	MainMenu	= GetMenu(Hwnd);
	FileMenu	= GetSubMenu(MainMenu, 0);
	HelpMenu	= GetSubMenu(MainMenu, 4);

	EnableMenuItem(FileMenu, ID_MENU_SUSPEND,	MF_BYCOMMAND|MF_ENABLED);
	EnableMenuItem(FileMenu, ID_MENU_RESUME,	MF_BYCOMMAND|MF_GRAYED);

	if(Core->LicenseService && Core->LicenseService->IsAllowed())
	{
		DeleteMenu(HelpMenu, ID_HELP_REGISTRATION, MF_BYCOMMAND);
	}
		
	if(Core->IsAdministrating)
	{
		LogsMenu = CreateMenu();

		MENUITEMINFO mii;
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_SUBMENU;
		mii.hSubMenu = LogsMenu;

		for(auto i : Core->Supervisor->Logs)
		{
			AddLog(i);
		}
		SetMenuItemInfo(MainMenu, 1, true, &mii);

		//

		DiagsMenu = CreateMenu();

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_SUBMENU;
		mii.hSubMenu = DiagsMenu;

		for(auto i : Core->Supervisor->Diagnosticss)
		{
			AddDiag(i);
		}
		SetMenuItemInfo(MainMenu, 2, true, &mii);
	}
	else
	{
		DeleteMenu(MainMenu, 1, MF_BYPOSITION);
	}
}

void CMmc::OnLogCreated(CLog * l)
{
	AddLog(l);
}

void CMmc::OnDiagnosticsCreated(CDiagnostic * d)
{
	AddDiag(d);
}
	
void CMmc::AddLog(CLog * log)
{
	Items.push_back(CMmcMenuItem());
	auto mi = &(Items.back());

	mi->Id			= NextId();
	mi->Log			= log;

	AppendMenu(LogsMenu, MF_STRING, mi->Id, log->GetName().c_str());

	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;
	mii.dwItemData = (INT_PTR)mi;

	SetMenuItemInfo(LogsMenu, mi->Id, false, &mii);

	for(auto i : Parameter->Many(L"LogView"))
	{
		if(i->Get<CString>() == log->GetName())
		{
			mi->Parameter = i;
			ShowLog(mi, true);
			break;
		}
	}
//	LogIdEnd++;
}
	

void CMmc::AddDiag(CDiagnostic * diag)
{
	Items.push_back(CMmcMenuItem());
	auto mi = &(Items.back());

	mi->Id			= NextId();
	mi->Diag		= diag;

	AppendMenu(DiagsMenu, MF_STRING, mi->Id, diag->GetName().c_str());

	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_DATA;
	mii.dwItemData = (INT_PTR)mi;

	SetMenuItemInfo(DiagsMenu, mi->Id, false, &mii);

	for(auto i : Parameter->Many(L"DiagView"))
	{
		if(i->Get<CString>() == diag->GetName())
		{
			mi->Parameter = i;
			ShowDiag(mi, true);
			break;
		}
	}
}

void CMmc::ShowLog(CMmcMenuItem * i, bool show)
{
	if(show && i->View == null)
	{
		if(i->Parameter == null)
		{
			i->Parameter = Parameter->Add(L"LogView");
		}
		i->View = new CLogView(Core, i->Log, i->Parameter->One(L"Rect")->Get<CGdiRect>(), this, i->Id);
		CheckMenuItem(LogsMenu, i->Id, MF_CHECKED);
	} 
	else if(!show && i->View != null)
	{
		i->Parameter->Set(i->Log->GetName());
		i->Parameter->One(L"Rect")->Set(i->View->GetRect());
		cleandelete(i->View);
		CheckMenuItem(LogsMenu, i->Id, MF_UNCHECKED);
	}
}
	
void CMmc::ShowDiag(CMmcMenuItem * i, bool show)
{
	if(show && i->View == null)
	{
		if(i->Parameter == null)
		{
			i->Parameter = Parameter->Add(L"DiagView");
		}
		i->View = new CDiagnosticView(Core, i->Diag, i->Parameter->One(L"Rect")->Get<CGdiRect>(), this, i->Id);
		CheckMenuItem(DiagsMenu, i->Id, MF_CHECKED);
	} 
	else if(!show && i->View != null)
	{
		i->Parameter->Set(i->Diag->GetName());
		i->Parameter->One(L"Rect")->Set(i->View->GetRect());
		cleandelete(i->View);
		CheckMenuItem(DiagsMenu, i->Id, MF_UNCHECKED);
	}
}
		
void CMmc::DestroyMenus()
{
	DestroyMenu(LogsMenu);
	DestroyMenu(DiagsMenu);
	DestroyMenu(ObjectsMenu);
}

