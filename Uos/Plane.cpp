#include "StdAfx.h"
#include "Plane.h"

using namespace uos;

CPlane::CPlane()
{
}

CPlane::CPlane(float nx, float ny, float nz)
{
	Normal.x = nx;
	Normal.y = ny;
	Normal.z = nz;

	Origin.Set(0, 0, 0);

	A = nx;
	B = ny;
	C = nz;
	D = 0;
}

CPlane::CPlane(CFloat3& origin, CFloat3& normal)
{
	Normal		= normal;
	Origin		= origin;
	A = normal.x;
	B = normal.y;
	C = normal.z;
	D = -(normal.x*origin.x + normal.y*origin.y + normal.z*origin.z);
}

CPlane::CPlane(CFloat3& p1, CFloat3& p2, CFloat3& p3)
{
	Origin = p1; 
	Normal = (p2-p1).Cross(p3-p1).GetNormalized();
	A = Normal.x;
	B = Normal.y;
	C = Normal.z;
	D = -(Normal.x*Origin.x + Normal.y*Origin.y + Normal.z*Origin.z);
}

bool CPlane::IsFrontFacingTo(CFloat3& direction)
{
	float dot = Normal.Dot(direction);
	return (dot <= 0);
}

float CPlane::GetSignedDistanceTo(CFloat3& point)
{
	return (point.Dot(Normal)) + D;
}

CFloat3 CPlane::Intersect(CRay & r)
{
	float t = -(A*r.Origin.x + B*r.Origin.y + C*r.Origin.z + D)/(A*r.Direction.x + B*r.Direction.y + C*r.Direction.z);
	
	return r.Origin + r.Direction * t;
}

float CPlane::GetAngle(CFloat3 & v)
{
	return asin(Normal.Dot(v)/v.Length());
}

