#include "stdafx.h"
#include "Angle.h"

using namespace uos;

void CAngle::AdjustRotationToPiRange(float * a)
{
	*a = fmod(*a, float(M_PI)*2.f);

	// find shortest rotate path 
	if(*a > M_PI)
		*a = *a - float(M_PI)*2.f;
	else if(*a < -M_PI)
		*a = *a + float(M_PI)*2.f;
}

float uos::CAngle::ToRadian(float a)
{
	return a * float(M_PI) / 180.f;
}
