#include "StdAfx.h"
#include "Float2.h"
#include "Converter.h"

using namespace uos;
using namespace DirectX;

const std::wstring	CFloat2::TypeName = L"float32.vector2";
const CFloat2		CFloat2::Nan = CFloat2(NAN, NAN);

std::wstring CFloat2::GetTypeName()
{
	return TypeName;
}

void CFloat2::Read(CStream * s)
{
	s->Read(&x, 2 * 4);
}

int64_t CFloat2::Write(CStream * s)  
{
	return s->Write(&x, 2 * 4);
}

void CFloat2::Write(std::wstring & s)
{
	s += CString::Format(L"%g %g", x, y);
}

void CFloat2::Read(const std::wstring & v)
{
	auto parts = CString(v).Split(L" ");
	x	= CFloat::Parse(parts[0]);
	y	= CFloat::Parse(parts[1]);
}

ISerializable * CFloat2::Clone()
{
	return new CFloat2(*this);
}
	
bool CFloat2::Equals(const ISerializable & a) const
{
	return (*this == (CFloat2 &)a) == TRUE;
}

bool CFloat2::operator==(const CFloat2 & a) const
{
	return x == a.x && y == a.y;
}

bool CFloat2::operator!=(const CFloat2 & a) const
{
	return x != a.x || y != a.y;
}

CFloat2 CFloat2::operator + (const CFloat2 & p) const
{
	return CFloat2(x+p.x, y+p.y);
}

CFloat2 CFloat2::operator - (const CFloat2 & v) const
{
	return CFloat2(x-v.x, y-v.y);
}

CFloat2 CFloat2::operator * (const CFloat2 & p) const
{
	XMVECTOR a = {x, y};
	XMVECTOR b = {p.x, p.y};

	auto r = XMVectorMultiply(a, b);
	return CFloat2(XMVectorGetX(r), XMVectorGetY(r));
}

CFloat2 CFloat2::operator * (const float a) const
{
	return CFloat2(x * a, y * a);
}

bool CFloat2::operator < (CFloat2 const & a)
{
	if(x != a.x) 
		return  x < a.x;	
	else
		return  y < a.y;
}

CFloat2::operator bool()
{
	return IsReal();
}

CFloat2::operator DirectX::XMFLOAT2 * ()
{
	return (DirectX::XMFLOAT2 *)(&x);
}

CFloat2::operator DirectX::XMFLOAT2 * () const
{
	return (DirectX::XMFLOAT2 *)(&x);
}

CString CFloat2::ToString()
{
	return CString::Format(L"%g %g", x, y);
}

void CFloat2::Set(float px, float py)
{
	x = px;
	y = py;
}
		
float CFloat2::GetLength()
{
	FXMVECTOR a = XMLoadFloat2(&XMFLOAT2(x, y));

	auto r = XMVector2Length(a);
	return r.m128_f32[0];
}

float CFloat2::GetLengthSq()
{
	return x*x + y*y;
}

bool CFloat2::IsReal()
{
	return std::isfinite(x) && std::isfinite(y);
}

float CFloat2::Dot(CFloat2 & v)
{
	float r;
	XMStoreFloat(&r, XMVector2Dot(XMLoadFloat2((XMFLOAT2 *)&x), XMLoadFloat2((XMFLOAT2 *)&v.x)));
	return r;
}

bool CFloat2::SegmentIntersection(CFloat2 start1, CFloat2 end1, CFloat2 start2, CFloat2 end2, CFloat2 *out_intersection)
{
	CFloat2 dir1 = end1 - start1;
	CFloat2 dir2 = end2 - start2;

	//считаем уравнения прямых проходящих через отрезки
	float a1 = -dir1.y;
	float b1 = +dir1.x;
	float d1 = -(a1*start1.x + b1*start1.y);

	float a2 = -dir2.y;
	float b2 = +dir2.x;
	float d2 = -(a2*start2.x + b2*start2.y);

	//подставляем концы отрезков, для выяснения в каких полуплоскотях они
	float seg1_line2_start = a2*start1.x + b2*start1.y + d2;
	float seg1_line2_end = a2*end1.x + b2*end1.y + d2;

	float seg2_line1_start = a1*start2.x + b1*start2.y + d1;
	float seg2_line1_end = a1*end2.x + b1*end2.y + d1;

	//если концы одного отрезка имеют один знак, значит он в одной полуплоскости и пересечения нет.
	if (seg1_line2_start * seg1_line2_end >= 0 || seg2_line1_start * seg2_line1_end >= 0) 
		return false;

	if(out_intersection)
	{
		float u = seg1_line2_start / (seg1_line2_start - seg1_line2_end);
		*out_intersection =  start1 + dir1 * u;
	}

	return true;
}


bool CFloat2::LineSegmentIntersection(CFloat2 la, CFloat2 lb, CFloat2 sstart, CFloat2 send, CFloat2 *out_intersection)
{
	CFloat2 dir1 = send - sstart;
	CFloat2 dir2 = lb - la;

	float a2 = -dir2.y;
	float b2 = +dir2.x;
	float d2 = -(a2*la.x + b2*la.y);

	//подставляем концы отрезков, для выяснения в каких полуплоскотях они
	float seg1_line2_start = a2*sstart.x + b2*sstart.y + d2;
	float seg1_line2_end = a2*send.x + b2*send.y + d2;

	//если концы одного отрезка имеют один знак, значит он в одной полуплоскости и пересечения нет.
	if (seg1_line2_start * seg1_line2_end >= 0) 
		return false;

	float u = seg1_line2_start / (seg1_line2_start - seg1_line2_end);
	*out_intersection =  sstart + dir1 * u;

	return true;
}

CFloat2 CFloat2::Cross(CFloat2 & v)
{
	//D3DXVec3Cross(&out, this, &v);
	CFloat2 r;
	XMStoreFloat2((XMFLOAT2 *)&r.x, XMVector2Cross(XMLoadFloat2((XMFLOAT2 *)&x), XMLoadFloat2((XMFLOAT2 *)&v.x)));
	return r;
}

float CFloat2::PointToRayDistance(CFloat2 & o, CFloat2 & d, CFloat2 & p) // o - origin of 'this' line
{
	//(|(x_2-x_1)x(x_1-x_0)|)/(|x_2-x_1|)
	return d.Cross(o - p).GetLength() / d.GetLength();
}

bool CFloat2::ProjectPointOntoSegment(CFloat2 & p, CFloat2 & v, CFloat2 & w, CFloat2 * pj, float * d)
{
	// Return minimum distance between line segment vw and point p
	float l2 = (v - w).GetLengthSq();  // i.e. |w-v|^2 -  avoid a sqrt

	if (l2 == 0.0) 
		return (p - v).GetLength();   // v == w case
				
	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	// We clamp t from [0,1] to handle points outside the segment vw.
	float t = (p - v).Dot(w - v) / l2;

	if(t < 0 || 1 < t)
		return false;

	*pj = v + (w - v) * t;  // Projection falls on the segment
	
	*d = (p - *pj).GetLength();

	return true;
}

CFloat2::operator D2D1_POINT_2F()
{
	return D2D1::Point2F(x, y);
}

float CFloat2::GetDistance(CFloat2 & v)
{
	return (*this - v).GetLength();
}
