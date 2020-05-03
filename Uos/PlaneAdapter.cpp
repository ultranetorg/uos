#include "StdAfx.h"
#include "PlaneAdapter.h"

using namespace uos;

const wchar_t * CPlaneAdapter::WORLDNODE_METANAME =  L"CPlaneAdapter";

CPlaneAdapter::CPlaneAdapter(IPlaneArranger * a)
{
	Arranger= a;
	Manual	= false;
	Arranged= false;
	X		= std::numeric_limits<float>::infinity();
	Y		= std::numeric_limits<float>::infinity();
	Distance= std::numeric_limits<float>::infinity();
}

CPlaneAdapter::~CPlaneAdapter()
{
}
/*

IArranger * CPlaneAdapter::GetArranger()
{
	return Arranger;
}
*/

CTransformation CPlaneAdapter::GetNormalTransformation()
{
	CTransformation s; 
	s.Position	= CFloat3(X, Y, Distance); 
	s.Rotation	= CQuaternion(0.f, 0.f, 0.f, 1);
	s.Scale		= 1.f;
	return s;
}

void CPlaneAdapter::SetNormalPosition(bool m, float d, float x, float y)
{
	Manual		= m;
	X			= x;
	Y			= y;
	Distance	= d;
	Arranged	= true;
}

bool CPlaneAdapter::IsManual()
{
	return Manual;
}

bool CPlaneAdapter::IsReal()
{
	return	FLT_MIN <= X && X <= FLT_MAX &&
			FLT_MIN <= Y && Y <= FLT_MAX &&
			FLT_MIN <= Distance && Distance <= FLT_MAX;
}
