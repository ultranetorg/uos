#include "stdafx.h"
#include "PolygonPositioning.h"

using namespace uos;

CPolygonalPositioning::CPolygonalPositioning()
{
	PlaceRandom =	[this](auto c, auto pp, auto & pk, auto & s)
					{
						//c = GetView()->GetCamera((pk.Camera ? pk.Camera : c)->Viewport);
						
						CFloat2 vpp = {	c->Viewport->W * 0.2f + c->Viewport->W * 0.6f * rand() / RAND_MAX,
										c->Viewport->H * 0.2f + c->Viewport->H * 0.6f * rand() / RAND_MAX};

						auto m = GetMatrix(c->Viewport);
						auto p = CPlane(0, 0, -1).Intersect(c->Raycast(vpp).Transform(!m));

						return CTransformation(p.x - s.W/2, p.y - s.H/2, 0);
					};

	PlaceCenter =	[this](auto c, auto pp, auto & pk, auto & s)
					{
						//c = GetView()->GetCamera((pk.Camera ? pk.Camera : c)->Viewport);

						auto m = GetMatrix(c->Viewport);
						auto p = CPlane(0, 0, -1).Intersect(c->Raycast().Transform(!m));

						return CTransformation(p.x - s.W/2, p.y - s.H/2, 0);
					};

	PlaceExact =	[this](auto c, auto pp, auto & pk, auto & s)
					{
						if(pk.Camera && pk.Vpp)
						{
							auto m = GetMatrix(pk.Camera->Viewport);
							auto p = CPlane(0, 0, -1).Intersect(pk.Camera->Raycast(pk.Vpp).Transform(!m))/*.VertexTransform(m)*/;
							return CTransformation(p.x - s.W/2, p.y - s.H/2, 0);
						}
						else
						{
							return PlaceRandom(c, pp, pk, s);
						}
					};

}

CFloat3 CPolygonalPositioning::GetPoint(CViewport * vp, CFloat2 & vpp)
{
	auto m = GetMatrix(vp);
	auto p = CPlane(0, 0, -1).Intersect(GetView()->GetCamera(vp)->Raycast(vpp).Transform(!m))/*.VertexTransform(m)*/;

	return CFloat3(p.x, p.y, 0);
}

CMatrix CPolygonalPositioning::GetMatrix(CViewport * vp)
{
	if(Transformation.Contains(vp))
	{
		return Transformation(vp)(vp);
	}
	else if(Transformation.Contains(null))
	{
		return Transformation(null)(vp);
	}
	else
		throw CException(HERE, L"No matrix assosiated");
}

CArray<CFloat2> CPolygonalPositioning::GetBounds(CViewport * vp)
{
	if(Bounds.Contains(vp))
	{
		return Bounds(vp)();
	}
	else if(Bounds.Contains(null))
	{
		return Bounds(null)();
	}
	else
		throw CException(HERE, L"No bounds assosiated");
}

CFloat3 CPolygonalPositioning::Project(CViewport * vp, CActiveSpace * s, CFloat3 & p)
{
	return GetView()->GetCamera(vp)->ProjectVertex(GetMatrix(vp).TransformCoord(p)); // new pos on screen
}

CPositioningCapture CPolygonalPositioning::Capture(CPick & pick, CSize & size, CFloat3 & offset)
{
	CPositioningCapture c;

	c.Space = pick.Space;
	c.Size = size;
	c.Offset = offset;

	auto v = GetView();
	auto m = GetMatrix(pick.Camera->Viewport);

	auto r = v->GetCamera(pick.Camera->Viewport)->Raycast(pick.Vpp).Transform(!m); // ortho ray (in Surface CS)
	c.Point = CPlane(0, 0, -1).Intersect(r); // intersection with Field XY plane (in Surface CS)
	
	return c;
}

CPositioningCapture CPolygonalPositioning::Capture(CPick & pick, CSize & size, CMatrix & wm)
{
	CPositioningCapture c;

	c.Size = size;
	c.Space = pick.Space;

	///auto wcp = pick.GetFinalPosition().VertexTransform(pick.Space->Matrix);
	auto r = pick.Camera->Raycast(pick.Vpp);
	auto wcp = CPlane(0, 0, -1).Intersect(r.Transform(wm.GetInversed())).VertexTransform(wm);

	auto m = GetMatrix(pick.Camera->Viewport);

	c.Point	 = wcp.VertexTransform(!m); // capture point on the node
	c.Offset = c.Point - pick.Space->Matrix.GetPosition().VertexTransform(!m); // from node (0,0) to capture point, in surface cs

	return c;
}

CTransformation CPolygonalPositioning::Move(CPositioningCapture & c, CPick & pick)
{
	auto v = GetView();
	auto m = GetMatrix(pick.Camera->Viewport);
	auto b = GetBounds(pick.Camera->Viewport);

	auto ray = v->GetCamera(pick.Camera->Viewport)->Raycast(pick.Vpp).Transform(!m);
	auto plane = CPlane(0, 0, -1);

	auto o = ray.Origin;
	auto d = ray.Direction;

	auto pp = plane.Intersect(ray);

	auto cp_pp = d;
	cp_pp.SetLength(c.Offset.z / sin(plane.GetAngle(d * -1.f))); // vector from pp to new capture point(and to eye)
																		
	auto cp = pp - cp_pp; // TEST NEEDED!!!!

	auto p = cp - c.Offset; // new position

	auto bound =	[&o, &b, &d](CFloat3 & p) -> CFloat3
					{
						CFloat2 q = {NAN, NAN};

						auto o_p = (o - p).GetNormalized(); // vector from eye to new position
		
						float dist_min = FLT_MAX;
						float dist;
						CFloat2 is;

						for(auto i=0u; i<b.size(); i++) // find intersections with edges
						{
							if(CFloat2::LineSegmentIntersection(p.ToXY(), p.ToXY() + CFloat2(1, 0), b[i], b[i<b.size()-1 ? i+1 : 0], &is))
							{
								dist = (p.ToXY() - is).GetLength();

								if(dist < dist_min)
								{
									q = is;
									dist_min = dist;
								}
							}

							if(CFloat2::LineSegmentIntersection(p.ToXY(), p.ToXY() + CFloat2(0, 1), b[i], b[i<b.size()-1 ? i+1 : 0], &is))
							{
								dist = (p.ToXY() - is).GetLength();

								if(dist < dist_min)
								{
									q = is;
									dist_min = dist;
								}
							}
						}

						if(dist_min == FLT_MAX)
						{
							//for(auto i=0u; i<b.size(); i++) // project p onto edges
							//{
							//	if(CFloat2::ProjectPointOntoSegment(p.ToXY(), b[i], b[i<b.size()-1 ? i+1 : 0], &is, &dist))
							//	{
							//		if(dist < dist_min)
							//		{
							//			q = is;
							//			dist_min = dist;
							//		}
							//	}
							//}

							for(auto & i : b) // project vertex onto o_d ray
							{
								auto dist = p.ToXY().GetDistance(i);

								if(dist < dist_min)
								{
									q = i;
									dist_min = dist;
								}
							}
						}

						//if(CPolygon::Contains(b, CFloat2(q.x, p.y)) || CPolygon::Contains(b, CFloat2(q.x, q.y)))
						{
							p.x = q.x;
						}
		
						//if(CPolygon::Contains(b, CFloat2(p.x, q.y)) || CPolygon::Contains(b, CFloat2(q.x, q.y)))
						{
							p.y = q.y;
						}

						return CFloat3(q.x, q.y, 0);
					};

	if(!CPolygon::Contains(b, p.ToXY()))
	{
		p = bound(p);
	}

	if(!CPolygon::Contains(b, CFloat2(p.x + c.Size.W, p.y)))
	{
		p = bound(p + CFloat3(c.Size.W, 0, 0));
		p.x -= c.Size.W;
	}

	if(!CPolygon::Contains(b, CFloat2(p.x, p.y + c.Size.H)))
	{
		p = bound(p + CFloat3(0, c.Size.H, 0));
		p.y -= c.Size.H;
	}

	///p.x = max(p.x, 0);
	///p.y = max(p.y, 0);
	
	///p.x = min(p.x, s.W);
	///p.y = min(p.y, s.H);

	//Level->Log->ReportDebug(this, L" x=%f   y=%f       Ares=%f   %f      x=%f   y=%f", _p.x, _p.y, Area.W, Area.H, p.x, p.y);

	return p;
}
