#include "StdAfx.h"
#include "InputDevice.h"

using namespace uos;

const CInputMessage CInputMessage::Empty = {};

CInputMessage::CInputMessage(CInputMessage && a)
{
	Id		= a.Id;
	Class	= a.Class;
	Control	= a.Control;
	Action	= a.Action;
	Screen	= a.Screen;
	Values	= a.Values;
	a.Values	= null;
}

CInputMessage::CInputMessage(const CInputMessage & a)
{
	Id		= a.Id;
	Class	= a.Class;
	Control	= a.Control;
	Action	= a.Action;
	Screen	= a.Screen;
	Values	= sh_assign(Values, a.Values);
}

CInputMessage::~CInputMessage()
{
	if(Values)
		Values->Free();
}

CInputMessage & CInputMessage::operator=(const CInputMessage & a)
{
	Id		= a.Id;
	Class	= a.Class;
	Control	= a.Control;
	Action	= a.Action;
	Screen	= a.Screen;
	Values	= sh_assign(Values, a.Values);

	return *this;
}