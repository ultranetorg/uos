#include "StdAfx.h"
#include "Quaternion.h"

using namespace uos;
using namespace DirectX;

CQuaternion::CQuaternion(float px, float py, float pz, float pw) : x(px), y(py), z(pz), w(pw)
{
}

CQuaternion::CQuaternion(float p, float y, float r)
{
	XMStoreFloat4(*this, XMQuaternionRotationRollPitchYaw(p, y, r));
}

CQuaternion::CQuaternion(CFloat3 & r)
{
	XMStoreFloat4(*this, XMQuaternionRotationRollPitchYaw(r.x, r.y, r.z));
}

CQuaternion::CQuaternion(CFloat3 & a, CFloat3 &b)
{
	auto c = a.Cross(b);
	x = c.x;
	y = c.y;
	z = c.z;
	w = sqrt(a.SquaredLength() * b.SquaredLength()) + a.Dot(b);
}

CQuaternion::CQuaternion(std::initializer_list<float> && l)
{
	auto i = l.begin();

	if(l.size() == 3)
	{
		XMStoreFloat4(*this, XMQuaternionRotationRollPitchYaw(i[0], i[1], i[2]));
	}
	if(l.size() == 4)
	{
		x = i[0];
		y = i[1];
		z = i[2];
		w = i[3];
	}
}

CFloat3 CQuaternion::ToEuler()
{
	CFloat3 o;
	auto q = Normalize();

	XMQuaternionToAxisAngle(&XMLoadFloat3(CFloat3(1, 0, 0)), &o.x, XMLoadFloat4(q));
	XMQuaternionToAxisAngle(&XMLoadFloat3(CFloat3(0, 1, 0)), &o.y, XMLoadFloat4(q));
	XMQuaternionToAxisAngle(&XMLoadFloat3(CFloat3(0, 0, 1)), &o.z, XMLoadFloat4(q));

	return o;
}

CQuaternion CQuaternion::Normalize()
{
	CQuaternion r;
	XMStoreFloat4(r, XMQuaternionNormalize(XMLoadFloat4(*this)));
	return r;
}
	
bool CQuaternion::IsIdentity()
{
	return x == 0 && y == 0 && z == 0 && w == 1;
}

CQuaternion CQuaternion::GetInversed()
{
	CQuaternion rr;
	XMStoreFloat4(rr, XMQuaternionInverse(XMLoadFloat4(*this)));
	return rr;
}

CQuaternion CQuaternion::operator * (const CQuaternion & q)
{
	CQuaternion rr;
	XMStoreFloat4(rr, XMQuaternionMultiply(XMLoadFloat4(*this), XMLoadFloat4(q)));
	return rr;
}
	
CQuaternion CQuaternion::operator * (const float  & s)
{
	return CQuaternion(x * s, y * s, z * s, w * s);
}
		
CQuaternion CQuaternion::operator + (const CQuaternion & a)
{
	return CQuaternion(x + a.x, y + a.y, z + a.z, w + a.w);
}

CQuaternion CQuaternion::operator - (const CQuaternion & a)
{
	return CQuaternion(x - a.x, y - a.y, z - a.z, w - a.w);
}

CString CQuaternion::ToString()
{
	return CString::Format(L"%g %g %g %g", x, y, z, w);
}

CString CQuaternion::ToNiceString()
{
	return CString::Format(L"%9g %9g %9g %9g", x, y, z, w);
}

bool CQuaternion::IsReal() const
{
	return std::isfinite(x) && std::isfinite(y) && std::isfinite(z) && std::isfinite(w);
}

void CQuaternion::Read(CStream * s)
{
	s->Read(&x, 4 * 4);
}

int64_t CQuaternion::Write(CStream * s)
{
	return s->Write(&x, 4 * 4);
}

CQuaternion CQuaternion::FromRotationAxis(CFloat3 & ax, float angle)
{
	CQuaternion q;
	XMStoreFloat4(q, XMQuaternionRotationAxis(XMLoadFloat3(ax), angle));
	return q;

}
