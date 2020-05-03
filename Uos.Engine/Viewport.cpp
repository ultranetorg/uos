#include "StdAfx.h"
#include "Viewport.h"

using namespace uos;

CViewport::CViewport(CEngineLevel * l) : CEngineEntity(l)
{
}

CViewport::~CViewport()
{
}

CFloat2 CViewport::TargetToViewport(CFloat2 & tp)
{
	return CFloat2(tp.x * W/TW, tp.y * H/TH);
}

CFloat2 CViewport::ScreenToTarget(CFloat2 & sp)
{
	return CFloat2(sp.x - TX, sp.y - TY);
}


CFloat2 CViewport::ScreenToViewport(CFloat2 & sp)
{
	return TargetToViewport(ScreenToTarget(sp));
}


bool CViewport::Contains(CFloat2 & sp)
{
	auto vpp = ScreenToViewport(sp);
	return CRect(0, 0, W, H).Contains(vpp);
}

