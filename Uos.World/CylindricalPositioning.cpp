#include "stdafx.h"
#include "CylindricalPositioning.h"

using namespace uos;

CCylindricalPositioning::CCylindricalPositioning(CWorldLevel * l, float h, float r)
{
	Level = l;
	H = h;
	R = r;

	PlaceRandom =	[this](auto c, auto pp, auto & pk, auto & s)
					{
						//c = pk.Camera ? pk.Camera : c;
						//c = GetView()->GetCamera(c->Viewport);

						CFloat2 vpp = {	c->Viewport->W * 0.2f + c->Viewport->W * 0.6f * rand() / RAND_MAX,
										c->Viewport->H * 0.2f + c->Viewport->H * 0.6f * rand() / RAND_MAX};

						auto m = GetMatrix(c->Viewport);
						auto p = GetPoint(c->Raycast(vpp).Transform(!m));

						return CMatrix::FromTransformation(CFloat3(-s.W/2, p.y-s.H/2, R),
															CQuaternion(0, CFloat3(0, 0, 1).GetAngleInXZ(p), 0),
															CFloat3(s.W/2, 0, -R),
															CFloat3(1)).Decompose();
					};

	PlaceCenter =	[this](auto c, auto pp, auto & pk, auto & s)
					{
						//c = pk.Camera ? pk.Camera : c;
						//c = GetView()->GetCamera(c->Viewport);

						auto m = GetMatrix(c->Viewport);
						auto p = GetPoint(c->Raycast().Transform(!m));

						return CMatrix::FromTransformation(CFloat3(-s.W/2, -s.H/2, R),
															CQuaternion(0, CFloat3(0, 0, 1).GetAngleInXZ(p), 0),
															CFloat3(s.W/2, 0, -R),
															CFloat3(1)).Decompose();
					};

	PlaceExact =	[this](auto c, auto pp, auto & pk, auto & s)
					{
						if(!pk.Camera)
						{
							return PlaceRandom(c, pp, pk, s);
						}

						c = pk.Camera;
						c = GetView()->GetCamera(c->Viewport);

						auto m = GetMatrix(c->Viewport);
						auto p = GetPoint(c->Raycast(pk.Vpp).Transform(!m));

						return CMatrix::FromTransformation(CFloat3(-s.W/2, p.y - s.H/2, R),
														   CQuaternion(0, CFloat3(0, 0, 1).GetAngleInXZ(p), 0),
														   CFloat3(s.W/2, 0, -R),
														   CFloat3(1)).Decompose();
					};


}

CMatrix CCylindricalPositioning::GetMatrix(CViewport * vp)
{
	if(Matrixes.Contains(vp))
	{
		return Matrixes(vp)(vp);
	}
	else if(Matrixes.Contains(null))
	{
		return Matrixes(null)(vp);
	}
	else
		throw CException(HERE, L"No matrix");
}

CFloat3 CCylindricalPositioning::GetDirection(CViewport * vp, CActive * node, CActiveSpace * s, CSize & size, CTransformation & t)
{
	auto im = !GetMatrix(vp);

	return ((CMatrix(t) * s->Matrix).TransformCoord(CFloat3(size.W/2, size.H/2, 0)) * im);
}

CFloat3 CCylindricalPositioning::GetPoint(CViewport * vp, CFloat2 & vpp)
{
	auto v = GetView();
	auto m = GetMatrix(vp);

	auto ray = v->GetCamera(vp)->Raycast(vpp).Transform(m.GetInversed());

	CFloat2 a, b;
	CFloat3 p;

	auto r0 = ray.Origin.ToXZ();
	auto r1 = (ray.Origin + ray.Direction).ToXZ();

	auto o = ray.Origin;
	auto r = ray.Direction;

	if(CCircle(CFloat2(0, 0), R).Intersect(r0, r1, &a, &b, 1.f) == 1)
	{
		p.x = a.x;
		p.z = a.y;
		p.y = r.y*(p.x - o.x) / r.x + o.y;
	}

	return p;
}

CFloat3 CCylindricalPositioning::GetPoint(CRay & ray)
{
	CFloat2 a, b;
	CFloat3 p;

	auto r0 = ray.Origin.ToXZ();
	auto r1 = (ray.Origin + ray.Direction).ToXZ();

	auto o = ray.Origin;
	auto r = ray.Direction;

	if(CCircle(CFloat2(0, 0), R).Intersect(r0, r1, &a, &b, 1.f) == 1)
	{
		p.x = a.x;
		p.z = a.y;

		if(r.x != 0)
			p.y = r.y*(p.x - o.x) / r.x + o.y;
		else
			p.y = r.y*(p.z - o.z) / r.z + o.y;
	}

	return p;
}

CPositioningCapture CCylindricalPositioning::Capture(CPick & pick, CSize & size, CFloat3 & offset)
{
	CPositioningCapture c;

	c.Size = size;

	auto v = GetView();
	auto m = GetMatrix(pick.Camera->Viewport);

	auto r = v->GetCamera(pick.Camera->Viewport)->Raycast(pick.Vpp).Transform(!m); // ray (in Surface CS)
	c.Point = CPlane(0, 0, -1).Intersect(r); // intersection with Field XY plane (in Surface CS)
	c.Offset = {0, 0, 0};

	return c;
}

CPositioningCapture CCylindricalPositioning::Capture(CPick & pick, CSize & size, CMatrix & wm)
{
	CPositioningCapture c;

	c.Size = size;

	//auto wcp = pick.GetWorldPosition();
	//auto ncp	= node->FinalMatrix.GetInversed().TransformCoord(wcp);
	auto im = GetMatrix(pick.Camera->Viewport).GetInversed();

	//c.Point		= wcp * im; // from eye to capture point on a node in surface cs
	c.Point = GetPoint(pick.Camera->Viewport, pick.Vpp);
	
	//c.Offset = c.Point - node->FinalMatrix.GetPosition() * im; // from node (0,0) to capture point in surface cs(!)
	c.Offset = wm.TransformCoord(CFloat3(size.W/2, size.H/2, 0)) * im;

	c.Angle = (wm.TransformCoord(CFloat3(size.W/2, size.H/2, 0)) * im).GetAngleInXZ(CFloat3(0, 0, 1));
	return c;
}

CTransformation CCylindricalPositioning::Move(CPositioningCapture & c, CPick & pick)
{
	auto p = GetPoint(pick.Camera->Viewport, pick.Vpp);

	//auto o = CPlane(0, 0, -1).Intersect(pick.Ray.Transform(node->FinalMatrix.GetInversed())) + CFloat3(c.Size.W/2, c.Size.H/2, 0);

	auto m = CMatrix::FromTransformation(CFloat3(-c.Size.W/2, -c.Size.H/2, R) + CFloat3(0, c.Offset.y, 0) + CFloat3(0, p.y - c.Point.y, 0), 
										 CQuaternion(0, -c.Angle + c.Point.GetAngleInXZ(p), 0), 
										 CFloat3(c.Size.W/2, 0, -R), 
										 CFloat3(1));

	auto t = m.Decompose();

	return t;
}

CFloat3 CCylindricalPositioning::Project(CViewport * vp, CActiveSpace * s, CFloat3 & p)
{
	return GetView()->GetCamera(vp)->ProjectVertex(Matrixes(vp)(vp).TransformCoord(p)); // new pos on screen
}

