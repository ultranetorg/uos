#include "stdafx.h"			
#include "Keyboard.h"
#include "InputSystem.h"

using namespace uos;

CKeyboard::CKeyboard(CEngineLevel * ew, CInputSystem * ie) : CEngineEntity(ew)
{
	InputEngine = ie;
}

CKeyboard::~CKeyboard()
{
}

bool CKeyboard::ProcessMessage(MSG * msg)
{
	CInputMessage m;
	m.Id		= InputEngine->GetNextID();
	m.Action	= EInputAction::Null;
	m.Class     = EInputClass::Keyboard;
	m.Control	= (EControl)msg->wParam;
	m.Device	= this;

	auto v = new CKeyboardInput();
	m.Values = v;

	v->WM		= msg->message;
	v->Flags	= msg->lParam;

	switch(msg->message)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			m.Action = EInputAction::On;
			break;
			
		case WM_KEYUP:
		case WM_SYSKEYUP:
			m.Action = EInputAction::Off;
			break;

		case WM_CHAR:
		case WM_SYSCHAR:
			m.Action = EInputAction::Char;
			break;
	}

	if(m.Action	!= EInputAction::Null)
	{
		InputEngine->SendInput(m);
	}

	return false;
}

EInputAction CKeyboard::GetPressState(EControl vk)
{
	USHORT  u= GetKeyState(int(vk));
	return (u & 0x8000) ? EInputAction::On : EInputAction::Off;
}

EInputAction CKeyboard::GetToggleState(EControl vk)
{
	USHORT  u= GetKeyState(int(vk));
	return (u & 0x0001) ? EInputAction::On : EInputAction::Off;
}
