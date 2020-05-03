#include "StdAfx.h"
#include "Float4.h"
#include "Rect.h"
#include "Matrix.h"

using namespace uos;
using namespace DirectX;

const CFloat4 CFloat4::Nan = CFloat4(NAN, NAN, NAN, NAN);
const std::wstring CFloat4::TypeName = L"float32.vector4";

CFloat4::CFloat4()
{
}

CFloat4::CFloat4(float xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw)
{
}

CFloat4::CFloat4(float px, float py, float pz, float pw) : x(px), y(py), z(pz), w(pw)
{
}

CFloat4::CFloat4(CFloat3 & v, float w) : CFloat4(v.x, v.y, v.z, w)
{
}

CFloat4::~CFloat4()
{
}

std::wstring CFloat4::GetTypeName()
{
	return TypeName;
}

void CFloat4::Read(CStream * s)
{
	s->Read(&x, 4 * 4);
}

int64_t CFloat4::Write(CStream * s)  
{
	return s->Write(&x, 4 * 4);
}

void CFloat4::Write(std::wstring & s)
{
	s += ToString();
}

void CFloat4::Read(const std::wstring & v)
{
	auto parts = CConverter::ParseArray<float>(v);
	x	= parts[0];
	y	= parts[1];
	z	= parts[2];
	w	= parts[3];
}

ISerializable * CFloat4::Clone()
{
	return new CFloat4(*this);
}

bool CFloat4::Equals(const ISerializable & a) const
{
	auto & b = (CFloat4 &)a;
	return b.x == x && b.y == y && b.z == z && b.w == w;
}

CString CFloat4::ToString()
{
	return CString::Format(L"%g %g %g %g", x, y, z, w);
}

CFloat3 & CFloat4::GetXYZ()
{
	return (CFloat3 &)*this;
}

CFloat2 & CFloat4::GetXY()
{
	return (CFloat2 &)*this;
}

bool CFloat4::IsReal()
{
	return isfinite(x) && isfinite(y) && isfinite(z) && isfinite(w);
}

CFloat4 CFloat4::Rand(float v)
{
	return CFloat4(v * rand()/32767, v * rand()/32767, v * rand()/32767, v * rand()/32767);
}

void CFloat4::Normalize()
{
	XMStoreFloat4(*this, XMVector4Normalize(XMLoadFloat4(*this)));
}

float CFloat4::Dot(CFloat4 & v)
{
	float r;
	XMStoreFloat(&r, XMVector4Dot(XMLoadFloat4(*this), XMLoadFloat4(v)));
	return r;
}