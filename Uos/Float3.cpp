#include "StdAfx.h"
#include "Float3.h"
#include "Matrix.h"

using namespace uos;
using namespace DirectX;

const CFloat3		CFloat3::Nan = CFloat3(NAN, NAN, NAN);
const std::wstring	CFloat3::TypeName = L"float32.vector3";

CString CFloat3::ToString()
{
	return CString::Format(L"%g %g %g", x, y, z);
}

std::wstring CFloat3::GetTypeName()
{
	return TypeName;
}

void CFloat3::Read(CStream * s)
{
	s->Read(&x, 3 * 4);
}

int64_t CFloat3::Write(CStream * s)  
{
	return s->Write(&x, 3 * 4);
}

void CFloat3::Write(std::wstring & s)
{
	s += CString::Format(L"%g %g %g", x, y, z);
}

void CFloat3::Read(const std::wstring & v)
{
	swscanf_s(v.data(), L"%f %f %f", &x, &y, &z);
}

ISerializable * CFloat3::Clone()
{
	return new CFloat3(*this);
}

bool CFloat3::Equals(const ISerializable & a) const
{
	auto & b = (CFloat3 &)a;
	return b.x == x && b.y == y && b.z == z;
}

CFloat3::CFloat3()
{
}

CFloat3::CFloat3(std::initializer_list<float> l)
{
	auto i = l.begin();

	x = *i++;
	y = *i++;
	z = *i;
}

CFloat3::CFloat3(float xyz)
{
	x = y = z = xyz;
}

CFloat3::CFloat3(float px, float py, float pz)
{
	x = (px);
	y = (py);
	z = (pz);
}

CFloat3::CFloat3(const CString & v)
{
	auto a = v.Split(L" ");
	x = CFloat::Parse(a[0]);
	y = CFloat::Parse(a[1]);
	z = CFloat::Parse(a[2]);
}

CFloat3::~CFloat3()
{
}

CString CFloat3::ToNiceString()
{
	return CString::Format(L"%9g %9g %9g", x, y, z);
}
	
CFloat2 CFloat3::ToXY()
{
	return CFloat2(x, y);
}

CFloat2 CFloat3::ToXZ()
{
	return CFloat2(x, z);
}

bool CFloat3::IsReal() const
{
	return (-FLT_MAX <= x && x <= FLT_MAX) && (-FLT_MAX <= y && y <= FLT_MAX) && (-FLT_MAX <= y && y <= FLT_MAX);
}

void CFloat3::Set(float px, float py, float pz)
{
	x = px;
	y = py;
	z = pz;
}

CFloat3 CFloat3::Cross(CFloat3 & v)
{
	//D3DXVec3Cross(&out, this, &v);
	CFloat3 r;
	XMStoreFloat3((XMFLOAT3 *)&r.x, XMVector3Cross(XMLoadFloat3((XMFLOAT3 *)&x), XMLoadFloat3((XMFLOAT3 *)&v.x)));
	return r;
}

float CFloat3::Dot(CFloat3 & v)
{
	float r;
	XMStoreFloat(&r, XMVector3Dot(XMLoadFloat3(*this), XMLoadFloat3(v)));
	return r;
}

float CFloat3::GetDistance(CFloat3 & v)
{
	return (*this - v).Length();
}

CFloat3 CFloat3::GetNormalized()
{
	//CFloat3 n = *this;
	//n.Normalize();
	//return n;
	CFloat3 r;
	XMStoreFloat3((XMFLOAT3 *)&r.x, XMVector3Normalize(XMLoadFloat3((XMFLOAT3 *)&x)));
	return r;
}

//void CFloat3::Normalize()
//{
//	D3DXVec3Normalize(this, this);
//
//	if(x > 1.f) x = 1.f;
//	if(y > 1.f) y = 1.f;
//	if(z > 1.f) z = 1.f;
//
//	if(x < -1.f) x = -1.f;
//	if(y < -1.f) y = -1.f;
//	if(z < -1.f) z = -1.f;
//}

float CFloat3::SquaredLength()
{
	//return D3DXVec3LengthSq(this);
	float r;
	XMStoreFloat(&r, XMVector3LengthSq(XMLoadFloat3((XMFLOAT3 *)&x)));
	return r;
}

float CFloat3::Length()
{
	float r;
	XMStoreFloat(&r, XMVector3Length(XMLoadFloat3((XMFLOAT3 *)&x)));
	return r;
}

void CFloat3::SetLength(float l)
{
	auto v = XMVector3Normalize(XMLoadFloat3((XMFLOAT3 *)&x));
	
	v = XMVectorMultiply(v, XMLoadFloat3(&XMFLOAT3(l, l, l)));
	
	XMStoreFloat3((XMFLOAT3 *)&x, v);
}

float CFloat3::GetLengthOfProjectionXZ()
{
	return sqrt(x*x + z*z);
}

float CFloat3::GetAngleInXZ()
{
	return atan2(x, z);
}

float CFloat3::GetAngleInYZ()
{
	return atan2(y, z);
//		return CMath::Sign(y) * acos(Dot(CFloat3(x, 0, z))/(Length() * CFloat3(x, 0, z).Length()));
}

float CFloat3::GetAngleInXY()
{
	return atan2(x, y);
}

float CFloat3::GetAngleInXZ(CFloat3 & a)
{
	return atan2(z*a.x - x*a.z, x*a.x + z*a.z);
}

float CFloat3::GetAngleInYZ(CFloat3 & a)
{
	return atan2(y*a.z - z*a.y, y*a.y + z*a.z);
}

float CFloat3::GetAngleInXY(CFloat3 & a)
{
	return atan2(x*a.y - y*a.x, x*a.x + y*a.y);
}

float CFloat3::GetAngle(CFloat3 & a)
{
	if((x!=0.f || y!=0.f || z!=0.f) && (a.x!=0.f || a.y!=0.f || a.z!=0.f))
	{
		float c = sqrt((x*x + y*y + z*z) * (a.x*a.x + a.y*a.y + a.z*a.z));
		float k = (x*a.x + y*a.y + z*a.z)/c;
		if(k > 1.f)		k = 1.f;
		if(k < -1.f)	k = -1.f;
		return acos(k);
	}
	else
	{
		return 0.f;
	}
}

float CFloat3::GetAngleToXY()
{
	if(x!=0.f || y!=0.f || z!=0.f)
	{
		float k = z/sqrt(x*x + y*y + z*z);
		if(k > 1.f)		k = 1.f;
		if(k < -1.f)	k = -1.f;
		return asin(k);
	}
	else
	{
		return 0.f;
	}
}

float CFloat3::GetAngleToXZ()
{
	if(x!=0.f || y!=0.f || z!=0.f)
	{
		float k = y/sqrt(x*x + y*y + z*z);
		if(k > 1.f)		k = 1.f;
		if(k < -1.f)	k = -1.f;
		return asin(k);
	}
	else
	{
		return 0.f;
	}
}		

float CFloat3::GetAngleToYZ()
{
	if(x!=0.f || y!=0.f || z!=0.f)
	{
		float k = x/sqrt(x*x + y*y + z*z);
		if(k > 1.f)		k = 1.f;
		if(k < -1.f)	k = -1.f;
		return asin(k);
	}
	else
	{
		return 0.f;
	}
}

CFloat3 CFloat3::ReplaceX(float v)
{
	return CFloat3(v, y, z);
}

CFloat3 CFloat3::ReplaceY(float v)
{
	return CFloat3(x, v, z);
}

CFloat3 CFloat3::ReplaceZ(float v)
{
	return CFloat3(x, y, v);
}

CFloat3 CFloat3::operator + (const float n) const
{
	return CFloat3(x+n, y+n, z+n);
}

CFloat3 CFloat3::operator - (const float n) const
{
	return CFloat3(x-n, y-n, z-n);
}

CFloat3 CFloat3::operator * (const float n) const
{
	return CFloat3(x*n, y*n, z*n);
}

CFloat3 CFloat3::operator / (const float n) const
{
	return CFloat3(x/n, y/n, z/n);
}

CFloat3 CFloat3::operator + (const CFloat3& v) const
{
	return CFloat3(x+v.x, y+v.y, z+v.z);
}

CFloat3 CFloat3::operator - (const CFloat3& v) const
{
	return CFloat3(x-v.x, y-v.y, z-v.z);
}

CFloat3 CFloat3::operator * (const CFloat3& v) const
{
	return CFloat3(x*v.x, y*v.y, z*v.z);
}

CFloat3 CFloat3::operator / (const CFloat3& v) const
{
	return CFloat3(x/v.x, y/v.y, z/v.z);
}

bool CFloat3::operator != (const CFloat3& a) const
{
	return a.x != x || a.y != y || a.z != z;
}	

bool CFloat3::operator==(const CFloat3& a) const
{
	return a.x == x && a.y == y && a.z == z;
}

CFloat3 CFloat3::operator * (const CMatrix & m) const // trasform coord
{
	CFloat3 r;
	XMStoreFloat3(r, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(x, y, z)), XMLoadFloat4x4(m)));
	return r;
}

//CFloat3 CFloat3::Transform(const CMatrix& m, float w)
//{
//	CFloat3 r;
//	XMStoreFloat3((XMFLOAT3 *)&r.x, XMVector4Transform(XMLoadFloat4(&XMFLOAT4(x, y, z, w)), XMLoadFloat4x4(m)));
//	return r;
//
//	//D3DXVECTOR4 out;
//	//D3DXVECTOR4 in(*this, w);
//	//D3DXVec4Transform(&out, &in, (const D3DXMATRIX *)&m);
//	//return CFloat3(out.x, out.y, out.z);
//}

//CFloat3 CFloat3::Transform(const CMatrix& m)
//{
//	CFloat3 r;
//	XMStoreFloat3(r, XMVector3Transform(XMLoadFloat3(&XMFLOAT3(x, y, z)), XMLoadFloat4x4(m)));
//	return r;
//
//	//D3DXVECTOR4 out;
//	//D3DXVec3Transform(&out, (D3DXVECTOR3 *)this, (const D3DXMATRIX *)&m);
//	//return CFloat3(out.x, out.y, out.z);
//}

void CFloat3::AdjustRotationTo2Pi()
{
	// eliminate superfluous rotations 
	x = fmod(x, float(M_PI)*2);
	y = fmod(y, float(M_PI)*2);
	z = fmod(z, float(M_PI)*2);

	if(x < 0.f)
		x = float(M_PI)*2 + x;
	if(y < 0.f)
		y = float(M_PI)*2 + y;
	if(z < 0.f)
		z = float(M_PI)*2 + z;
}

void CFloat3::AdjustRotationToPiRange()
{
	// eliminate superfluous rotations 
	x = fmod(x, float(M_PI)*2);
	y = fmod(y, float(M_PI)*2);
	z = fmod(z, float(M_PI)*2);

	// find shortest rotate path 
	if(x > M_PI)
		x = x - float(M_PI)*2;
	else if(x < -M_PI)
		x = x + float(M_PI)*2;

	if(y > M_PI)
		y = y - float(M_PI)*2;
	else if(y < -M_PI)
		y = y + float(M_PI)*2;

	if(z > M_PI)
		z = z - float(M_PI)*2;
	else if(z < -M_PI)
		z = z + float(M_PI)*2;	
}

float CFloat3::GetDistanceToPoint(CFloat3 & o, CFloat3 & p) // o - origin of 'this' line
{
	//(|(x_2-x_1)x(x_1-x_0)|)/(|x_2-x_1|)
	return Cross(o - p).Length()/Length();
}

float CFloat3::LineToLineDistance(CFloat3 & A, CFloat3 & B, CFloat3 & C, CFloat3 & D, CFloat3 & Pos1, CFloat3 & Pos2)
{
	CFloat3 p1,p2,w,Norm,V;
	float t1,t2,a,b,c,d,e,f,H;
	p1=B-A;
	p2=D-C;
	w=A-C;
	a = p1.Dot(p1);
	b = p1.Dot(p2);
	c = p2.Dot(p2);
	d = p1.Dot(w);
	e = p2.Dot(w);
	f=(a*c)-(b*b);

	t1=((b*e)-(c*d))/f;
	t2=((a*e)-(b*d))/f;

	Pos1 = A + p1*t1;
	Pos2 = C + p2*t2;
	V = Pos1-Pos2;
	H = V.Length();
        return H;
}

float CFloat3::RayToRayDistance(CFloat3 & A, CFloat3 & p1, CFloat3 & C, CFloat3 & p2, CFloat3 & Pos1, CFloat3 & Pos2)
{
	CFloat3 /*p1,p2,*/w,Norm,V;
	float t1,t2,a,b,c,d,e,f,H;
	//p1=B-A;
	//p2=D-C;
	w=A-C;
	a = p1.Dot(p1);
	b = p1.Dot(p2);
	c = p2.Dot(p2);
	d = p1.Dot(w);
	e = p2.Dot(w);
	f=(a*c)-(b*b);

	t1=((b*e)-(c*d))/f;
	t2=((a*e)-(b*d))/f;

	Pos1=A + p1 * t1;
	Pos2=C + p2 * t2;
	V = Pos1-Pos2;
	H = V.Length();
    return H;
}

CRay CRay::Transform(const CMatrix& m)
{
	CRay ray;
	ray.Origin		= m.TransformCoord(Origin);
	ray.Direction	= m.TransformNormal(Direction).GetNormalized();
	
	return ray;		
}

bool CRay::IsReal()
{
	return Origin.IsReal() && Direction.IsReal();
}

float CFloat3::RayToPointDistance(CFloat3 & o, CFloat3 & d, CFloat3 & p) // o - origin of 'this' line
{
	//(|(x_2-x_1)x(x_1-x_0)|)/(|x_2-x_1|)
	return d.Cross(o - p).Length() / d.Length();
}

CFloat3 CFloat3::VertexTransform(CMatrix & m) 
{
	CFloat3 r;
	XMStoreFloat3(r, XMVector3TransformCoord(XMLoadFloat3(*this), XMLoadFloat4x4(m)));
	return r;
	//CFloat3 out;
	//D3DXVec3TransformCoord(&out, &v, this);
	//return out;
}

CFloat3 CFloat3::NormalTransform(CMatrix & m) 
{
	CFloat3 r;
	XMStoreFloat3(r, XMVector3TransformNormal(XMLoadFloat3(*this), XMLoadFloat4x4(m)));
	return r;
	//CFloat3 out;
	//D3DXVec3TransformCoord(&out, &v, this);
	//return out;
}

bool CFloat3::operator<(CFloat3 const & a)
{
	if(x != a.x) return  x < a.x;	else
	if(y != a.y) return  y < a.y;	else
				return  z < a.z;
}

CFloat3::operator bool()
{
	return IsReal();
}

CFloat3::operator DirectX::XMFLOAT3 * ()
{
	return (DirectX::XMFLOAT3 *)(&x);
}

CFloat3::operator DirectX::XMFLOAT3 * () const
{
	return (DirectX::XMFLOAT3 *)(&x);
}