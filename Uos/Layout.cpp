#include "stdafx.h"
#include "Layout.h"

using namespace uos;

EYAlign uos::ToYAlign(const CString & name)
{
	if(name == L"Top")		return EYAlign::Top; else
	if(name == L"Center")	return EYAlign::Center; else
	if(name == L"Bottom")	return EYAlign::Bottom; else
		
	return EYAlign::Null;
}

EXAlign uos::ToXAlign(const CString & name)
{
	if(name == L"Left")		return EXAlign::Left; else
	if(name == L"Center")	return EXAlign::Center; else
	if(name == L"Right")	return EXAlign::Right; else
		
	return EXAlign::Null;
}

EDirection uos::ToDirection(const CString & name)
{
	if(name == L"X")	return EDirection::X; else
	if(name == L"Y")	return EDirection::Y; else
	if(name == L"Z")	return EDirection::Z; else
		
	return EDirection::Null;
}


