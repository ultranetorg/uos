#include "StdAfx.h"
#include "ScreenEngine.h"

using namespace uos;

CScreenEngine::CScreenEngine(CEngineLevel * l, CDirectSystem * ge) : CEngineEntity(l)
{
	DisplaySystem	= ge;
	Parameter		= l->Config->Root->One(L"ViewEngine");

	PcUpdate		= new CPerformanceCounter(L"ViewEngine update");
	Level->Core->AddPerformanceCounter(PcUpdate);
		
	for(auto i : Parameter->Many(L"Layout"))
	{
		Layouts.push_back(i->AsString());
	}

	Level->Core->Suspended		+= ThisHandler(OnLevel1Suspended);
	Level->Core->Resumed		+= ThisHandler(OnLevel1Resumed);
	Level->Core->ExitQueried	+= ThisHandler(OnLevel1ExitQueried);

}
	
CScreenEngine::~CScreenEngine()
{

	Level->Core->ExitQueried	-= ThisHandler(OnLevel1ExitQueried);
	Level->Core->Suspended		-= ThisHandler(OnLevel1Suspended);
	Level->Core->Resumed		-= ThisHandler(OnLevel1Resumed);
		
	delete PcUpdate;

	//for(auto i : Targets)
	//{
	//	GraphicEngine->RemoveTarget(i);
	//}
	
	for(auto i : Screens)
	{
		delete i;
	}
}

void CScreenEngine::OnLevel1Suspended()
{
	for(auto i : Screens)
	{
		i->Show(false);
	}
}

void CScreenEngine::OnLevel1Resumed()
{
	for(auto i : Screens)
	{
		i->Show(true);
	}
}

void CScreenEngine::OnLevel1ExitQueried()
{
	for(auto i : Screens)
	{
		i->Show(false);
	}
}

void CScreenEngine::SetLayout(const CString & layoutName)
{
	SetLayout(Parameter->Many(L"Layout"), layoutName);
}
	
void CScreenEngine::SetLayout(CArray<CXon *> & ls, const CString & layoutName)
{
	for(auto i : Screens)
	{
		delete i;
	}

	Screens.clear();

	CXon * layout = null;
	for(auto i : ls)
	{
		if(i->AsString() == layoutName)
		{
			layout = i;
			break;
		}
	}
						
	if(layout->Many(L"Screen").front()->Get<CString>(L"DisplayDevice") == L"*")
	{
		for(auto i : DisplaySystem->Displays)
		{
			CreateScreen(layout->Many(L"Screen").front(), i);
		}
	}
	else
	{
		for(auto i : layout->Many(L"Screen"))
		{
			CreateScreen(i, null);
		}		
	}
		
	PrimaryScreen = null;
	if(layout->Get<CString>(L"Primary") == L"Auto")
	{
		PrimaryScreen = GetScreenByRect(DisplaySystem->GetDefaultDevice()->NRect);
	}
	else
	{
		PrimaryScreen = GetScreenByName(layout->Get<CString>(L"Primary")); // primary viewppot explicitly defined
	}

	if(!PrimaryScreen && !Screens.empty()) // primary still not defined, set first as primary
	{
		PrimaryScreen = Screens.front();
	}

	if(!PrimaryScreen)
	{
		SetLayout(L"Default");
	}
}

void CScreenEngine::CreateScreen(CXon * p, CDisplayDevice * dd)
{
	auto getrect =	[this](CDisplayDevice * dd, CXon * x) -> CGdiRect
					{
						CGdiRect r;

						auto s = x->Get<CString>(L"Size").Split(L" ");
						auto p = x->Get<CString>(L"Position").Split(L" ");

						if(s.size() == 2)
						{
							if(s[0] == L"Device")	r.Width = dd->NRect.Width;	else
							if(s[0] == L"Desktop")	r.Width = dd->NRectWork.Width;
							else
							{
								r.Width = CInt32::Parse(s[0]);
								r.Width = int(r.Width * DisplaySystem->Scaling.x);
							}

							if(s[1] == L"Device")	r.Height = dd->NRect.Height; else
							if(s[1] == L"Desktop")	r.Height = dd->NRectWork.Height;
							else
							{
								r.Height = CInt32::Parse(s[1]);
								r.Height = int(r.Height * DisplaySystem->Scaling.y);
							}
						}
						if(p.size() == 2)
						{
							if(p[0] == L"Device")	r.X = dd->NRect.X + (dd->NRect.Width - r.Width)/2 ;	else
							if(p[0] == L"Desktop")	r.X = dd->NRectWork.X + (dd->NRectWork.Width - r.Width)/2;
							else
							{
								r.X = CInt32::Parse(p[0]);
								r.X = int(r.X * DisplaySystem->Scaling.x);
							}
				
							if(p[1] == L"Device")	r.Y = dd->NRect.Y + (dd->NRect.Height - r.Height)/2; else
							if(p[1] == L"Desktop")	r.Y = dd->NRectWork.Y + (dd->NRectWork.Height - r.Height)/2;
							else
							{
								r.Y = CInt32::Parse(p[1]);
								r.Y = int(r.Y * DisplaySystem->Scaling.y);
							}
						}

						return r;
					};


	auto type = p->One(L"Type")->AsEnum<EScreenType>();
		
	switch(type)
	{
		case EScreenType::Display:
		{
			///if(dd == null)
			///{
			///	dd = GraphicEngine->GetDevice(p->One(L"DisplayDevice")->Get<CString>());
			///	if(dd == null)
			///	{
			///		Level->Log->ReportMessage(this, L"Display not found: %s", p->One(L"DisplayDevice")->Get<CString>().c_str());
			///		return;
			///	}
			///}
			///vp = new CViewport(Level, GraphicEngine, dd);
			break;
		}
		case EScreenType::Window:
		{
			auto dd	= DisplaySystem->GetDisplayDevice(p->One(L"DisplayDevice")->Get<CString>());
			if(dd == null)
			{
				Level->Log->ReportMessage(this, L"Display not found: %s", p->One(L"DisplayDevice")->Get<CString>());
				return;
			}
			auto r	= getrect(dd, p);
			auto rw	= dd->NRectWork;
			
			auto name = Level->Core->Product.HumanName + L" - Screen: " + CString::Format(L"%d", Screens.size());
			auto w	  = new CWindowScreen(Level, 0, name.c_str(), WS_VISIBLE|WS_POPUP|(p->Get<CBool>(L"WindowCaption") ? WS_CAPTION|WS_THICKFRAME|WS_SYSMENU : 0), r.X, r.Y, r.Width, r.Height, null, null, null, null);

			if(p->Value)
			{
				w->Name = p->Get<CString>();
			}

			w->Tags = p->Get<CString>(L"Tags").SplitToList(L" ");

			w->BringToTop();

			Screens.push_back(w);
			ScreenAdded(w);
			break;
		}
	}
}

CScreen * CScreenEngine::GetScreenByRect(CGdiRect & r)
{
	float max = 0;
	CScreen * vp = null;
	for(auto i : Screens)
	{
		auto ir = i->Rect.Intersect(r);
		if(float(ir.Width * ir.Height) > max)
		{
			max = float(ir.Width * ir.Height);
			vp = i;
		}
	}
	return vp;
}
	
CScreen * CScreenEngine::GetScreenByName(const CString & name)
{
	for(auto i : Screens)
	{
		//CViewport * vp = (*i);
		if(i->Name == name)
		{
			return i;
		}
	}
	return null;
}

CScreen * CScreenEngine::GetScreenByDisplayDeviceName(const CString & n)
{
/*		foreach(i, Viewports)
	{
		CViewport * vp = (*i);
		if(vp->DisplayDevice != null)
		{
			if(vp->DisplayDevice->Name == n)
			{
				return vp;
			}
		}
	}*/

	return null;
}

CScreen * CScreenEngine::GetScreenByWindow(HWND hwnd)
{
	for(auto i : Screens)
	{
		if(auto w = dynamic_cast<CWindowScreen *>(i))
		{
			if(w->GetHwnd() == hwnd)
				return i;
		}
	}
	return null;
}

void CScreenEngine::Update()
{
	PcUpdate->BeginMeasure();

	PcUpdate->EndMeasure();
}

void CScreenEngine::TakeScreenshot(CString const & suffix)
{
	throw CException(HERE, L"Not implemented");
// 	if(suffix.find(L'-') != CString::npos)
// 	{
// 		throw CException(HERE, L"Hyphen (-) not allowed");
// 	}
// 
// 	auto dir = Level->Nexus->Storage->CreateGlobalDirectory(L"Screenshots");
// 
// 	auto d = Level->Nexus->Storage->OpenDirectory(dir);
// 
// 	int session = -1;
// 
// 	for(auto i : d->FindByRegex(L".+-(.+)-(\\d+)-\\w+"))
// 	{
// 		if(i.Matches[1] == suffix)
// 		{
// 			int s = CInt32::Parse(i.Matches[2]);
// 			if(s > session)
// 			{
// 				session = s;
// 			}
// 		}
// 	}
// 
// 	session++;
// 
// 	Level->Nexus->Storage->Close(d);
// 	
// 	for(auto t : Targets)
// 	{
// 		CString name = CString::Format(L"%s-%s-%04d-%016X.png", Level->Core->Product.Version.ToString(), suffix, session, t);
// 		t->TakeScreenshot(CUfl::Join(CFile::GetClassName(), dir, name));
// 	}
}

CScreen * CScreenEngine::GetPrimaryScreen()
{
	return PrimaryScreen;
}


void CScreenEngine::OnDiagnosticsUpdate(CDiagnosticUpdate & a)
{
}
