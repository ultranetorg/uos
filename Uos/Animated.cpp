#include "StdAfx.h"
#include "Animated.h"

using namespace uos;

CAnimatedAngles::CAnimatedAngles(const CFloat3 & b, const CFloat3 & e, CAnimation & a) : CAnimated(b, e, a)
{
	Begin.AdjustRotationToPiRange();
	End.AdjustRotationToPiRange();
	
	// find shortest path
	if(End.x - Begin.x > M_PI)
		End.x -= float(M_PI)*2.f;
	else if(End.x - Begin.x < -M_PI)
		End.x += float(M_PI)*2.f;

	if(End.y - Begin.y > M_PI)
		End.y -= float(M_PI)*2.f;
	else if(End.y - Begin.y < -M_PI)
		End.y += float(M_PI)*2.f;

	if(End.z - Begin.z > M_PI)
		End.z -= float(M_PI)*2.f;
	else if(End.z - Begin.z < -M_PI)
		End.z += float(M_PI)*2.f;
}
