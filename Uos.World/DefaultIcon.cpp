#include "stdafx.h"
#include "DefaultIcon.h"

using namespace uos;

CDefaultIcon::CDefaultIcon(CWorld * l, CString const & name) : CIcon(l, l->Server, name)
{
	SetContentFromMaterial(Level->Materials->GetMaterial(L"1 1 1"));
}

CDefaultIcon::~CDefaultIcon()
{
}

void CDefaultIcon::SetEntity(CUol & e)
{
	Entity = null;
}
