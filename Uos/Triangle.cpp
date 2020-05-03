#include "stdafx.h"
#include "Triangle.h"

using namespace uos;

//?????? ?????? ????????????
CTriIntersection CTriangle::LegIntersection(CFloat3 & v1, CFloat3 & v2, CFloat3 & v3, CFloat3 & begin, CFloat3 & end)
{
	CTriIntersection out;

	CFloat3 p1,p2,p3,p4;
	float t,u,v,det;

	out.Return=false;
	p1=begin - v1;
	p2=begin - end;
	p3=v2 - v1;
	p4=v3 - v1;

	det=p2.Dot(p3.Cross(p4));
	if(det==0)
	{
		out.Return=false;
		return out;
	}

	t=p1.Dot(p3.Cross(p4))/det;
	if((t<0) || (t>1))
	{
		out.Return=false;
		return out;
	}
	u=p2.Dot(p1.Cross(p4))/det;
	if((u<0) || (u>1))
	{
		out.Return=false;
		return out;
	}

	v=p2.Dot(p3.Cross(p1))/det;
	if((v<0) || (v>1))
	{
		out.Return=false;
		return out;
	}
	if((u+v)>1)
	{
		out.Return=false;
		return out;
	}

	out.Point = v1 + p3*u + p4*v;
	//Result.Point.X=PointPlane1.X+p3.X*u+p4.X*v;
	//Result.Point.Y=PointPlane1.Y+p3.Y*u+p4.Y*v;
	//Result.Point.Z=PointPlane1.Z+p3.Z*u+p4.Z*v;


	p2= p2 * -1.f;
	p2 = p2.GetNormalized();
	out.Vector=p2;
	out.Distance=begin.GetDistance(out.Point);
	out.Return=true;
	return out;
}

CTriIntersection CTriangle::SphereIntersection(CTriangle & treagle, TSpher & Spher, CFloat3 & posSpher)
{
	CTriIntersection Result;

	CFloat3 offset,Normal,EndPoint;
	CTriIntersection Intersect;
//		float MinD;
	CFloat3 e1,e2;
	Result.Return=false;

	Normal=treagle.Normals[3]; //???????? ??????????? ??????? ???????

	EndPoint = posSpher - Normal * Spher.r; //??????? ????????? ????? ????? ? ???????

	//???????? ???????????? ?????? ? ????????, ?????? ????? ?????? ??? ??????? ??????? ?????, ?????? ????? EndPoint
	Intersect=LegIntersection(treagle.Vertices[0],treagle.Vertices[1],treagle.Vertices[2],posSpher,EndPoint);

	if(Intersect.Return)//???? ?????? ?????????? ?????? => ????? ????:)
	{
		Result.Vector=Normal; //?????? ???????
		Result.Point=Intersect.Point;//????? ???????????
		Result.Distance=Intersect.Distance;//????????? ?? ???????
		Result.Return=true;
	}
	return Result;
}	

float sign_(CFloat2 & p1, CFloat2 & p2, CFloat2 & p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool CTriangle::ContainsPoint(CFloat2 & a, CFloat2 & b, CFloat2 & c, CFloat2 & p)
{
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = sign_(p, a, b);
	d2 = sign_(p, b, c);
	d3 = sign_(p, c, a);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

bool CTriangle::Intersect(CFloat3 & a, CFloat3 & b, CFloat3 & c, CRay & ray, bool testCull, CTriIntersection * is)
{
	//#define TEST_CULL
	#define EPSILON FLT_EPSILON

	CFloat3 edge1, edge2, tvec, pvec, qvec;
	float det, inv_det, u, v, d;
	

	edge1 = b - a;/* find vectors for two edges sharing vert0 */
	edge2 = c - a;

	pvec = ray.Direction.Cross(edge2);/* begin calculating determinant - also used to calculate U parameter */

	det = edge1.Dot(pvec);/* if determinant is near zero, ray lies in plane of triangle */

	if(testCull)
	{
		if(det < EPSILON)
			return false;
	
		tvec = ray.Origin - a;/* calculate distance from vert0 to ray origin */
	
		u = tvec.Dot(pvec);/* calculate U parameter and test bounds */
		if(u < 0.0 || u > det)
			return false;
	
		qvec = tvec.Cross(edge1);/* prepare to test V parameter */
	
		v = ray.Direction.Dot(qvec);/* calculate V parameter and test bounds */
		if(v < 0.0 || u + v > det)
			return false;
	
		inv_det = 1 / det;
		d = edge2.Dot(qvec) * inv_det; /* calculate t, scale parameters, ray intersects triangle */
	
		if(d < 0)
			return false;

		if(is)
		{
			is->Distance = d;
	
			is->U = u * inv_det;
			is->V = v * inv_det;
		}
	} 
	else
	{
		/* the non-culling branch */
		if(det > -EPSILON && det < EPSILON)
			return false;

		inv_det = 1.0f / det;

		/* calculate distance from vert0 to ray origin */
		tvec = ray.Origin - a;

		/* calculate U parameter and test bounds */
		u = tvec.Dot(pvec) * inv_det;
		if(u < 0.0 || u > 1.0)
			return false;

		/* prepare to test V parameter */
		qvec = tvec.Cross(edge1);

		/* calculate V parameter and test bounds */
		v = ray.Direction.Dot(qvec) * inv_det;
		if(v < 0.0 || u + v > 1.0)
			return false;

		/* calculate t, ray intersects triangle */
		d = edge2.Dot(qvec) * inv_det;

		if(is)
		{
			is->Distance = d;

			is->U = u;
			is->V = v;
		}
	}

	return true;
}

