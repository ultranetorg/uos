#include "stdafx.h"			
#include "KeyboardInputDriver8.h"

using namespace uos;

#if 0
CKeyboardInputDriver8::CKeyboardInputDriver8(CEngineLevel * ew, LPDIRECTINPUT8 di) : CEngineEntity(ew)
{
	Exclusive	= false;
	Messages.reserve(1024);
	
	for(int i=0; i<256; i++)
	{
		Buttons[i] = 0;
	}
	
	Wnd = GetDesktopWindow();

	DIPROPDWORD bufsize;
	bufsize.diph.dwSize       = sizeof(DIPROPDWORD);
	bufsize.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	bufsize.diph.dwObj        = 0;
	bufsize.diph.dwHow        = DIPH_DEVICE;
	bufsize.dwData            = 256;

/*		DIPROPDWORD axismod;
	axismod.diph.dwSize       = sizeof(DIPROPDWORD);
	axismod.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	axismod.diph.dwObj        = 0;
	axismod.diph.dwHow        = DIPH_DEVICE;
	axismod.dwData            = DIPROPAXISMODE_ABS;*/

	Verify(di->CreateDevice(GUID_SysKeyboard, &DIDevice, null));
	Verify(DIDevice->SetDataFormat(&c_dfDIKeyboard));
	Verify(DIDevice->SetCooperativeLevel(Wnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
	Verify(DIDevice->SetProperty(DIPROP_BUFFERSIZE, &bufsize.diph));
	Verify(DIDevice->Acquire());
	
	Level->Core->Suspended	+= ThisHandler(OnSuspended);
	Level->Core->Resumed	+= ThisHandler(OnResumed);
}

CKeyboardInputDriver8::~CKeyboardInputDriver8()
{
	Level->Core->Suspended	-= ThisHandler(OnSuspended);
	Level->Core->Resumed	-= ThisHandler(OnResumed);

	DIDevice->Unacquire();
	DIDevice->Release();
}

CArray<CInputMessage> & CKeyboardInputDriver8::GetMessages()
{
	return Messages;
}

void CKeyboardInputDriver8::OnSuspended()
{
	DIDevice->Unacquire();
}

void CKeyboardInputDriver8::OnResumed()
{
	Verify(DIDevice->Acquire());
}

void CKeyboardInputDriver8::OnMouseEvent()
{
}

void CKeyboardInputDriver8::Read()
{
	CInputMessage m;
	Messages.clear();
	
	DIDEVICEOBJECTDATA dod[256];
	DWORD kn = _countof(dod);

	m.Class	= EInputClass::KeyboardButton;

	while(DIDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), dod, &kn, 0) != S_OK)
	{
		DIDevice->Acquire();
	}

	for(int i=0; i<(int)kn; i++)
	{
		Buttons[dod[i].dwOfs]	= dod[i].dwData;
	}
	
	for(int i=0; i<256; i++)
	{
		if(Buttons[i] & 0x80)
		{
			m.Sender						= (EInputSender)i;
			m.Action						= Buttons[i] ? EInputAction::On : EInputAction::Off;
			m.Value[0]						= Buttons[i];
			Messages.push_back(m);
		}
	}
	
/*
	if(Messages.size() > 0)
	{
		MessageReceived(Messages);
	}*/

}

EInputAction CKeyboardInputDriver8::GetControlState(int vk)
{
	int sc = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
	return Buttons[sc] & 0x80 ? EInputAction::On : EInputAction::Off;
}


void CKeyboardInputDriver8::SetExclusiveMode()
{
	Verify(DIDevice->Unacquire());
	Verify(DIDevice->SetCooperativeLevel(Wnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE));
	Verify(DIDevice->Acquire());
	Exclusive = true;
}

void CKeyboardInputDriver8::SetNoneExclusiveMode()
{
	Verify(DIDevice->Unacquire());
	Verify(DIDevice->SetCooperativeLevel(Wnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE ));
	Verify(DIDevice->Acquire());
	Exclusive = false;
}
#endif