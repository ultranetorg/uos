#include "stdafx.h"
#include "Polygon.h"

using namespace uos;

bool CPolygon::Real(CArray<CFloat2> & v)
{
	for(auto & i : v)
	{
		if(!i.IsReal())
		{
			return false;
		}
	}
	return true;
}

bool CPolygon::Contains(CArray<CFloat2> & v, CFloat2 & p)
{
	for(auto i = 1u; i<v.size()-1; i++)
	{
		if(CTriangle::ContainsPoint(v[0], v[i], v[i+1], p))
		{
			return true;
		}
	}
	return false;
}

CFloat2 CPolygon::MinXVertex(CArray<CFloat2> & vertexes)
{
	CFloat2 v(FLT_MAX, NAN);

	for(auto & i : vertexes)
	{
		if(i.x < v.x)
		{
			v = i;
		}
	}

	return v;
}

CFloat2 CPolygon::MaxXVertex(CArray<CFloat2> & vertexes)
{
	CFloat2 v(FLT_MIN, NAN);

	for(auto & i : vertexes)
	{
		if(i.x > v.x)
		{
			v = i;
		}
	}

	return v;
}

CFloat2 CPolygon::MinYVertex(CArray<CFloat2> & vertexes)
{
	CFloat2 v(NAN, FLT_MAX);

	for(auto & i : vertexes)
	{
		if(i.y < v.y)
		{
			v = i;
		}
	}

	return v;
}

CFloat2 CPolygon::MaxYVertex(CArray<CFloat2> & vertexes)
{
	CFloat2 v(NAN, FLT_MIN);

	for(auto & i : vertexes)
	{
		if(i.y > v.y)
		{
			v = i;
		}
	}

	return v;
}
