#include "stdafx.h"			
#include "Mouse.h"
#include "InputSystem.h"

using namespace uos;

CMouse::CMouse(CEngineLevel * ew, CScreenEngine * se, CInputSystem * ie) : CEngineEntity(ew)
{
	ScreenEngine = se;
	InputEngine = ie;

	Default = LoadCursor(null, IDC_ARROW);

}

CMouse::~CMouse()
{
}

void CMouse::ResetImage()
{
	Image = null;
}

void CMouse::SetImage(HCURSOR c)
{
	Image = c;
}

void CMouse::ApplyCursor()
{
	for(auto i : ScreenEngine->Screens)
	{
		i->SetCursor(Image ? Image : Default);
	}
}

bool CMouse::ProcessMessage(MSG * msg)
{
	CInputMessage m;
	m.Device	= this;
	m.Action	= EInputAction::Null;
	m.Class		= EInputClass::Mouse;
	m.Screen	= ScreenEngine->GetScreenByWindow(msg->hwnd);
	m.Id		= InputEngine->GetNextID();

	auto v = m.MakeValues<CMouseInput>();

	POINT cp;

	switch(msg->message)
	{
		case WM_MOUSEMOVE:
			m.Control		= EControl::Cursor;
			m.Action		= EInputAction::Move;
			
			cp = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
			break;
		
		case WM_MOUSEWHEEL:
			m.Control	= EControl::Wheel;
			m.Action	= EInputAction::Rotation;
			
			cp.x = GET_X_LPARAM(msg->lParam);
			cp.y = GET_Y_LPARAM(msg->lParam);
			ScreenToClient(msg->hwnd, &cp);
			
			v->RotationDelta.y	= float(GET_WHEEL_DELTA_WPARAM(msg->wParam) / WHEEL_DELTA);
			break;
		
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			m.Control		= msg->message == WM_LBUTTONDOWN ? EControl::LeftButton : (msg->message == WM_RBUTTONDOWN ? EControl::RightButton : EControl::MiddleButton) ;
			m.Action		= EInputAction::On;
			cp	= {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			m.Control		= msg->message == WM_LBUTTONUP ? EControl::LeftButton : (msg->message == WM_RBUTTONUP ? EControl::RightButton : EControl::MiddleButton) ;
			m.Action		= EInputAction::Off;
			cp = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
			break;

		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
			m.Control		= msg->message == WM_LBUTTONDBLCLK ? EControl::LeftButton : (msg->message == WM_RBUTTONDBLCLK ? EControl::RightButton : EControl::MiddleButton) ;
			m.Action		= EInputAction::DoubleClick;
			cp = {GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam)};
			break;
	}

	if(m.Action	!= EInputAction::Null)
	{
		auto p = m.Screen->NativeToScreen(CFloat2(float(cp.x), float(cp.y)));

		if(m.Screen != Screen)
		{
			Screen = m.Screen;
			Position = p;
		}

		v->Position.x		= p.x;
		v->Position.y		= p.y;
		v->PositionDelta	= {p.x - Position.x, p.y - Position.y};
			
		Position = p;
	
		InputEngine->SendInput(m);
	}
			
	return false;
}

EInputAction CMouse::GetKeyState(EControl vk)
{
	USHORT  u= GetAsyncKeyState(int(vk));
	return (u & 0x8000) ? EInputAction::On : EInputAction::Off;
}

