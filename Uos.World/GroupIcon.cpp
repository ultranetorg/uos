#include "stdafx.h"
#include "GroupIcon.h"

using namespace uos;

CGroupIcon::CGroupIcon(CWorld * l, CString const & name) : CIcon(l, l->Server, name)
{
	Level = l;
	SetContentFromImage(Server->MapPath(L"Group-24x24.png"));
}

CGroupIcon::~CGroupIcon()
{
}
