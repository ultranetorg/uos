#include "StdAfx.h"
#include "BBox.h"

using namespace uos;

const CAABB CAABB::InversedMax = CAABB(FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
const CAABB CAABB::Empty = CAABB(0, 0, 0);

CAABB::CAABB()
{
}

CAABB::CAABB(float w, float h, float d)
{
	w /= 2;
	h /= 2;
	d /= 2;
	Min.Set(-w, -h, -d);
	Max.Set(w, h, d);
}

CAABB::CAABB(CFloat3 a, CFloat3 b)
{
	Min = a;
	Max = b;
}

CAABB::CAABB(float minx, float miny, float minz, float maxx, float maxy, float maxz) : Min(minx, miny, minz), Max(maxx, maxy, maxz)
{
}

CAABB::~CAABB()
{
}

float CAABB::GetWidth()
{
	return Max.x - Min.x;
}

float CAABB::GetHeight()
{
	return Max.y - Min.y;

}

float CAABB::GetDepth()
{
	return Max.z - Min.z;
}

CAABB CAABB::Join(CAABB & a)
{
	CAABB o;

	o.Min.x = min(Min.x, a.Min.x);
	o.Min.y = min(Min.y, a.Min.y);
	o.Min.z = min(Min.z, a.Min.z);

	o.Max.x = max(Max.x, a.Max.x);
	o.Max.y = max(Max.y, a.Max.y);
	o.Max.z = max(Max.z, a.Max.z);

	return o;
}

CAABB CAABB::Cross(CAABB & a)
{
	CAABB bb;
	bb.Min.x = max(Min.x, a.Min.x);
	bb.Min.y = max(Min.y, a.Min.y);
	bb.Min.z = max(Min.z, a.Min.z);
						  
	bb.Max.x = min(Max.x, a.Max.x);
	bb.Max.y = min(Max.y, a.Max.y);
	bb.Max.z = min(Max.z, a.Max.z);

	return bb;
}

bool CAABB::Contains(CFloat3 & p)
{
	return	Min.x <= p.x && p.x <= Max.x &&
			Min.y <= p.y && p.y <= Max.y &&
			Min.z <= p.z && p.z <= Max.z;
}

bool CAABB::Contains(CFloat3 & p, float e)
{
	return	Min.x - e <= p.x && p.x <= Max.x + e &&
			Min.y - e <= p.y && p.y <= Max.y + e &&
			Min.z - e <= p.z && p.z <= Max.z + e;
}

CString CAABB::ToNiceString()
{
	return CString::Format(L"%s %s", Min.ToNiceString(), Max.ToNiceString());
}

void CAABB::Join2D(CTransformation & t, CSize & a)
{
	Min.x = min(Min.x, t.Position.x);
	Min.y = min(Min.y, t.Position.y);
	Min.z = min(Min.z, t.Position.z);

	//auto p = CMatrix(i->Transformation).TransformCoord(CFloat3(i->Area.Width, i->Area.Height, i->Area.Depth));
	auto p = t.Position + CFloat3(a.W, a.H, a.D);

	Max.x = max(Max.x, p.x);
	Max.y = max(Max.y, p.y);
	Max.z = max(Max.z, p.z);
}

CSize CAABB::GetSize()
{
	auto w = Max.x - Min.x;
	auto h = Max.y - Min.y;
	auto d = Max.z - Min.z;
	return CSize(isfinite(w) && w > 0 ? w : 0, isfinite(h) && h > 0 ? h : 0, isfinite(d) && d > 0 ? d : 0);
}

CAABB CAABB::Transform(CMatrix const & m)
{
	CFloat3 p[8];
	p[0] = m.TransformCoord(CFloat3(Min.x, Min.y, Min.z));
	p[1] = m.TransformCoord(CFloat3(Min.x, Min.y, Max.z));
	p[2] = m.TransformCoord(CFloat3(Min.x, Max.y, Min.z));
	p[3] = m.TransformCoord(CFloat3(Min.x, Max.y, Max.z));
	p[4] = m.TransformCoord(CFloat3(Max.x, Min.y, Min.z));
	p[5] = m.TransformCoord(CFloat3(Max.x, Min.y, Max.z));
	p[6] = m.TransformCoord(CFloat3(Max.x, Max.y, Min.z));
	p[7] = m.TransformCoord(CFloat3(Max.x, Max.y, Max.z));
	
	CAABB o;
	o.Min.x = min(min(min(p[0].x, p[1].x), min(p[2].x, p[3].x)), min(min(p[4].x, p[5].x), min(p[6].x, p[7].x)));
	o.Min.y = min(min(min(p[0].y, p[1].y), min(p[2].y, p[3].y)), min(min(p[4].y, p[5].y), min(p[6].y, p[7].y)));
	o.Min.z = min(min(min(p[0].z, p[1].z), min(p[2].z, p[3].z)), min(min(p[4].z, p[5].z), min(p[6].z, p[7].z)));
	
	o.Max.x = max(max(max(p[0].x, p[1].x), max(p[2].x, p[3].x)), max(max(p[4].x, p[5].x), max(p[6].x, p[7].x)));
	o.Max.y = max(max(max(p[0].y, p[1].y), max(p[2].y, p[3].y)), max(max(p[4].y, p[5].y), max(p[6].y, p[7].y)));
	o.Max.z = max(max(max(p[0].z, p[1].z), max(p[2].z, p[3].z)), max(max(p[4].z, p[5].z), max(p[6].z, p[7].z)));

	return o;
}

