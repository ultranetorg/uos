#include "stdafx.h"			
#include "TouchScreen.h"
#include "InputSystem.h"

using namespace uos;

CTouchScreen::CTouchScreen(CEngineLevel * ew, CInputSystem * ie, CWindowScreen *w ) : CEngineEntity(ew)
{
	InputEngine = ie;
	Screen = w;
	
	int value = GetSystemMetrics(SM_DIGITIZER);
	
	if(value & NID_READY)
	{
	}
	if(value  & NID_MULTI_INPUT)
	{
	}
	if(value & NID_INTEGRATED_TOUCH)
	{
	}

	RegisterTouchWindow(w->Hwnd, 0);

	//auto hr = CoCreateInstance(CLSID_ManipulationProcessor, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (VOID**)(&ManipProcessor));
	//ManipulationEventSink	= new CTouchManipulation(InputEngine, this, ManipProcessor, Screen);
}

CTouchScreen::~CTouchScreen()
{
   UnregisterTouchWindow(Screen->Hwnd);

   //delete ManipulationEventSink;
   //ManipProcessor->Release();
}

bool CTouchScreen::ProcessMessage(MSG * msg)
{
	//Touches.clear();
	Inputs.resize(LOWORD(msg->wParam));

	CTouchInput * v = null;

	if(GetTouchInputInfo((HTOUCHINPUT)msg->lParam, UINT(Inputs.size()), Inputs.data(), sizeof(TOUCHINPUT)))
	{
		for(auto & i : Inputs)
		{
			//#ifdef _DEBUG
			//	InputEngine->Level->Log->ReportDebug(InputEngine, L"%5d   %10d %10d    %c", i.dwID, i.x, i.y, i.dwFlags & TOUCHEVENTF_PRIMARY ? 'P' : ' ');
			//#endif

			auto t = Touches.Find([&i](auto & j){ return j->Id == i.dwID; });

			if(i.dwFlags & TOUCHEVENTF_DOWN)
			{
				if(i.dwFlags & TOUCHEVENTF_PRIMARY)
				{
					for(auto i : Touches)
						delete i;

					Touches.clear();
				}

				t = new CTouch{int(i.dwID), bool(i.dwFlags & TOUCHEVENTF_PRIMARY)};
				Touches.push_back(t);

				v = new CTouchInput();
				v->Action = ETouchAction::Added;

				//ManipProcessor->ProcessDown(i.dwID, float(i.x), float(i.y));
			}
			if(i.dwFlags & TOUCHEVENTF_UP)
			{
				v = new CTouchInput();
				v->Action = ETouchAction::Removed;


				//ManipProcessor->ProcessUp(i.dwID, float(i.x), float(i.y));
			}
			if(i.dwFlags & TOUCHEVENTF_MOVE)
			{
				v = new CTouchInput();
				v->Action = ETouchAction::Movement;

				//ManipProcessor->ProcessMove(i.dwID, float(i.x), float(i.y));
			}

			if(v)
			{
				CInputMessage m;
				m.Class		= EInputClass::TouchScreen;
				m.Control	= EControl::Screen;
				m.Action	= EInputAction::Null;
				m.Id		= InputEngine->GetNextID();	
				m.Screen	= Screen;
				m.Device	= this;
				m.Values	= v;

				POINT p = {i.x/100, i.y/100};
				ScreenToClient(Screen->Hwnd, &p);

				v->Touches = &Touches;
				v->Touch = t;
				t->Position = Screen->NativeToScreen(CFloat2(float(p.x), float(p.y)));
				
				if(i.dwFlags & TOUCHEVENTF_DOWN)
				{
					t->Origin = t->Position;
				}

				if(i.dwMask & TOUCHEVENTFMASK_CONTACTAREA)
					t->Size = {float(i.cxContact)/100, float(i.cyContact)/100};
				else
					t->Size = CFloat2::Nan;

				InputEngine->SendInput(m);

				if(i.dwFlags & TOUCHEVENTF_UP)
				{
					Touches.Remove(t);
					delete t;
				}
			}

		}

		CloseTouchInputHandle((HTOUCHINPUT)msg->lParam);
	}
	else
	{
		// GetLastError() and error handling
	}
	

	//#ifdef _DEBUG
	//int n = 0;
	//for(auto & i : Touches)
	//{
	//	InputEngine->Level->Log->ReportDebug(InputEngine, L"%5d   %5d   %10f %10f    %10f %10f    %c", n++, i->Id, i->Position.x, i->Position.y, i->Size.x, i->Size.y, i->Primary ? L'P' : L' ');
	//}
	//#endif

	return v != null;
}
