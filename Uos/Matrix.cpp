#include "StdAfx.h"
#include "Matrix.h"
#include "Float4.h"
#include "Float3.h"

using namespace uos;
using namespace DirectX;

const std::wstring CMatrix::TypeName = L"float32.matrix4x4";

const CMatrix CMatrix::Nan	= {	NAN, NAN, NAN, NAN,	 
								NAN, NAN, NAN, NAN,	
								NAN, NAN, NAN, NAN,
								NAN, NAN, NAN, NAN};

const CMatrix CMatrix::Identity = {	1, 0, 0, 0,		
									0, 1, 0, 0,		
									0, 0, 1, 0, 
									0, 0, 0, 1};

CMatrix::CMatrix()
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] = NAN;
}

CMatrix::CMatrix(std::initializer_list<float> l)
{
	auto k = l.begin();
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] = *k++;
}

CMatrix::CMatrix(float x, float y, float z)
{
	*this = FromPosition(x, y, z);
}

CMatrix::CMatrix(float s)
{
	*this = FromScaling(s, s, s);
}

CMatrix::CMatrix(float x, float y, float z, float w) : CMatrix(CQuaternion(x, y, z, w))
{
}

CMatrix::CMatrix(const CQuaternion & q)
{
	auto v = XMLoadFloat4(&XMFLOAT4(q.x, q.y, q.z, q.w));

	XMStoreFloat4x4(*this, XMMatrixRotationQuaternion(v));
	//D3DXMatrixRotationQuaternion(this, &q);
}

CMatrix::CMatrix(const CTransformation & t)
{
	if(t.IsReal())
	{
		CMatrix m = CMatrix::Identity;

		if(t.Scale.x != 1.f || t.Scale.y != 1.f || t.Scale.z != 1.f)
		{
			m = FromScaling(t.Scale);
		}
		if(t.Rotation.x != 0.f || t.Rotation.y != 0.f || t.Rotation.z != 0.f || t.Rotation.w != 1)
		{
			m = m * CMatrix(t.Rotation);
		}
		if(t.Position.x != 0.f || t.Position.y != 0.f || t.Position.z != 0.f)
		{
			m = m * FromPosition(t.Position);
		}
		*this = m;
	}
	else
	{
		this->CMatrix::CMatrix();
	}
}

CMatrix::CMatrix(CFloat3 & t, CQuaternion & r)
{
	*this = CMatrix(r) * CMatrix::FromPosition(t);
}

CMatrix::~CMatrix()
{
}

CMatrix::CMatrix(const ISerializable & a)
{
	*this = dynamic_cast<CMatrix &>(const_cast<ISerializable &>(a));
}

void CMatrix::operator=(const ISerializable & a)
{
	this->CMatrix::CMatrix(a);
}
	
bool CMatrix::operator!=(const CMatrix & a)
{
	return memcmp(a.m, m, sizeof(m)) != 0;
}

std::wstring CMatrix::GetTypeName()
{
	return TypeName;
}

void CMatrix::Read(CStream * s)
{
	s->Read(m, 16 * 4);
}

int64_t CMatrix::Write(CStream * s)  
{
	return s->Write(m, 16 * 4);
}

void CMatrix::Write(std::wstring & s)
{
	s += CString::Format(L"%g %g %g %g %g %g %g %g %g %g %g %g %g %g %g %g",m[0][0], m[0][1], m[0][2], m[0][3], 
																			m[1][0], m[1][1], m[1][2], m[1][3], 
																			m[2][0], m[2][1], m[2][2], m[2][3], 
																			m[3][0], m[3][1], m[3][2], m[3][3]);
}

void CMatrix::Read(const std::wstring & v)
{
	auto p = CString(v).Split(L" ");

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] = CFloat::Parse(p[i*4 + j]);
}

ISerializable * CMatrix::Clone()
{
	return new CMatrix(*this);
}

bool CMatrix::Equals(const ISerializable & a) const
{
	return memcmp(((CMatrix &)a).m, m, sizeof(m)) == 0;
}

CMatrix CMatrix::FromPosition(const CFloat3 & p)
{
	return FromPosition(p.x, p.y, p.z);
}

CMatrix CMatrix::FromPosition(float x, float y, float z)
{
	CMatrix m = CMatrix::Identity;
	m._41 = x;
	m._42 = y;
	m._43 = z;
	return m;
	//D3DXMatrixTranslation(&m, x, y, z);
	//return m;
}

CMatrix CMatrix::FromScaling(const CFloat3 & s)
{
	return FromScaling(s.x, s.y, s.z);
}

CMatrix CMatrix::FromScaling(float x, float y, float z)
{
	CMatrix r;
	XMStoreFloat4x4((XMFLOAT4X4 *)r.m, XMMatrixScaling(x, y, z));
	return r;
}

CMatrix CMatrix::FromEuler(CFloat3 & r)
{
	return FromEuler(r.x, r.y, r.z);
}

CMatrix CMatrix::FromEuler(float y, float p, float r)
{
	//CMatrix m;
	//D3DXMatrixRotationYawPitchRoll(&m, x, y, z);
	//return m;
	CMatrix m;
	XMStoreFloat4x4(m, XMMatrixRotationRollPitchYaw(p, y, r));
	return m;
}

CMatrix CMatrix::FromRotationAxis(CFloat3 & ax, float angle)
{
	CMatrix m;
	XMStoreFloat4x4(m, XMMatrixRotationAxis(XMLoadFloat3(ax), angle));
	return m;

}

CMatrix CMatrix::FromPerspectiveFovLH(float yfov, float aspect, float znear, float zfar)
{
	CMatrix r;
	XMStoreFloat4x4(r, XMMatrixPerspectiveFovLH(yfov, aspect, znear, zfar));
	return r;
}

CMatrix CMatrix::FromOrthographicOffCenterLH(float l, float r, float b, float t, float znear, float zfar)
{
	CMatrix rr;
	XMStoreFloat4x4(rr, XMMatrixOrthographicOffCenterLH(l, r, b, t, znear, zfar));
	return rr;
}

CMatrix CMatrix::FromLookToLH(CFloat3 & eye, CFloat3 & dir, CFloat3 & up)
{
	CMatrix rr;
	XMStoreFloat4x4(rr, XMMatrixLookToLH(XMLoadFloat3(eye), XMLoadFloat3(dir), XMLoadFloat3(up)));
	return rr;
}

bool CMatrix::IsReal()
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<4; j++)
		{
			if( ! (-FLT_MAX <= m[i][j] && m[i][j] <= FLT_MAX))
			{
				return false;
			}
		}
	}
	return true;
}

CMatrix CMatrix::GetInversed() const
{
	CMatrix xm;
	XMStoreFloat4x4((XMFLOAT4X4 *)xm.m, XMMatrixInverse(null, XMLoadFloat4x4((XMFLOAT4X4 *)m)));
	return xm;

	//CMatrix out;
	//D3DXMatrixInverse(&out, null, this);
	//return out;
}

CMatrix CMatrix::GetTransposed()
{
	CMatrix xm;
	XMStoreFloat4x4((XMFLOAT4X4 *)xm.m, XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4 *)m)));
	return xm;

	//CMatrix out;
	//D3DXMatrixTranspose(&out, this);
	//return out;
}

CTransformation CMatrix::Decompose()
{
	XMVECTOR s, r, p;
	XMMatrixDecompose(&s, &r, &p, XMLoadFloat4x4(*this));

	CTransformation t;

	XMStoreFloat3(t.Scale, s);
	XMStoreFloat4(t.Rotation, r);
	XMStoreFloat3(t.Position, p);

	return t;

	//CTransformation t;
	//D3DXMatrixDecompose(&t.Scale, &t.Rotation, &t.Position, this);
	//return t;
}


CQuaternion CMatrix::ToQuaternion()
{
	CQuaternion q;
	XMStoreFloat4(q, XMQuaternionRotationMatrix(XMLoadFloat4x4((XMFLOAT4X4 *)m)));
	return q;
}

//void CMatrix::MakeScaling(float sx, float sy, float sz)
//{
//	D3DXMatrixScaling(this, sx, sy, sz);
//}
//	
//void CMatrix::MakeTranslation(float x, float y, float z)
//{
//	D3DXMatrixTranslation(this, x, y, z);
//}
//
//void CMatrix::MakeRotationYawPitchRoll(CFloat3 & r)
//{
//	D3DXMatrixRotationYawPitchRoll(this, r.x, r.y, r.z);
//}
//
//void CMatrix::MakeRotationX(float angle)
//{
//	D3DXMatrixRotationX(this, angle);
//}
//
//void CMatrix::MakeRotationY(float angle)
//{
//	D3DXMatrixRotationY(this, angle);
//}
//
//void CMatrix::MakeRotationZ(float angle)
//{
//	D3DXMatrixRotationZ(this, angle);
//}

CMatrix CMatrix::FromTransformation(CFloat3 const & t, CQuaternion const & r, CFloat3 const & rc, CFloat3 const & s)
{
	CMatrix xm;
	XMStoreFloat4x4(xm, XMMatrixAffineTransformation(XMLoadFloat3(s), XMLoadFloat3(rc), XMLoadFloat4(r), XMLoadFloat3(t)));
	return xm;

	//D3DXMatrixAffineTransformation(this, s.x, &rc, &r, &t);
}

//void CMatrix::MakeAffineTransformation(CFloat3 & t, CFloat3 & r, CFloat3 & rc, CFloat3 & s)
//{
//	SetTransformation(t, CQuaternion::FromEuler(r), rc, s);
//}

CFloat3 CMatrix::TransformCoord(CFloat3 & v) const
{
	CFloat3 r;
	XMStoreFloat3(r, XMVector3TransformCoord(XMLoadFloat3(v), XMLoadFloat4x4(*this)));
	return r;
	//CFloat3 out;
	//D3DXVec3TransformCoord(&out, &v, this);
	//return out;
}

CFloat3 CMatrix::TransformNormal(CFloat3 & v) const
{
	CFloat3 r;
	XMStoreFloat3(r, XMVector3TransformNormal(XMLoadFloat3(v), XMLoadFloat4x4(*this)));
	return r;
	//CFloat3 out;
	//D3DXVec3TransformCoord(&out, &v, this);
	//return out;
}

CMatrix CMatrix::operator * (const CMatrix& m)  const
{
	CMatrix r;
	XMStoreFloat4x4(r, XMMatrixMultiply(XMLoadFloat4x4(*this), XMLoadFloat4x4(m)));
	return r;
}

void CMatrix::operator *= (const CMatrix& m)
{
	XMStoreFloat4x4(*this, XMMatrixMultiply(XMLoadFloat4x4(*this), XMLoadFloat4x4(m)));
}

CFloat4 CMatrix::operator * (const CFloat4& v) 
{
	CFloat4 out;
	
	out.x = _11*v.x + _12*v.y + _13*v.z + _14*v.w;
	out.y = _21*v.x + _22*v.y + _23*v.z + _24*v.w;
	out.z = _31*v.x + _32*v.y + _33*v.z + _34*v.w;
	out.w = _41*v.x + _42*v.y + _43*v.z + _44*v.w;
	
	return out;
}

CFloat3 CMatrix::GetPosition()
{
	return CFloat3(_41, _42, _43);
}

/*
CFloat3 CMatrix::GetRotation()
{

	float thetaY, thetaZ, thetaX;
	
	
		// Assuming the angles are in radians.
	if  (_31  <  +1)
	{
		if  (_31  >  -1)
		{
			thetaY  =  asin(-_31);
			thetaZ  =  atan2(_21,_11);
			thetaX  =  atan2(_32,_33);
		}
		else  //  r20  =  -1
		{
			//  Not  a  unique  solution:    thetaX  -  thetaZ  =  atan2(-r12,r11)
			thetaY  =  +M_PI/2;
			thetaZ  =  atan2(-_23,_22);
			thetaX  =  0;
		}
	}
	else  //  r20  =  +1
	{
		//  Not  a  unique  solution:    thetaX  +  thetaZ  =  atan2(-r12,r11)
		thetaY  =  -M_PI/2;
		thetaZ  =  atan2(-_23,_22);
		thetaX  =  0;
	}
	return CFloat3(thetaY, thetaZ, thetaX);
	
	CFloat3 vec(_12, _22, _32);
	CFloat3 xv(_11,_21, _31);  
	return CFloat3(-atan2(vec.z, sqrt(vec.x*vec.x + vec.y*vec.y)), xv.Dot(CFloat3(0,0,1)), -atan2(-vec.x,vec.y));

	//and we have our reference axis vectors:
	CFloat3 vR(1, 0, 0);// (right axis)
	CFloat3 vU(0, 1, 0);// (up axis)
	CFloat3 vL(0, 0, 1);// (look axis)
	
	//... and here, "dot product" comes to help us :)
	CFloat3 kR(_11,_21, 0);
	CFloat3 kU(0,	_22,_32);
	CFloat3 kL(_13,  0,_33);
	
	float y = acos(kR.Dot(vR) / kR.Length());
	float p = acos(kU.Dot(vU) / kU.Length());
	float r = acos(kL.Dot(vL) / kL.Length());
	
	return CFloat3(y, p, r);

}
*/
