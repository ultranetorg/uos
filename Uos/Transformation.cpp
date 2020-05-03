#include "StdAfx.h"
#include "Transformation.h"
#include "Matrix.h"

using namespace uos;

const CTransformation	CTransformation::Nan = CTransformation(NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN);
const CTransformation	CTransformation::Identity = CTransformation(0, 0, 0, 0, 0, 0, 1, 1, 1, 1);
const std::wstring		CTransformation::TypeName = L"transformation";

CTransformation::CTransformation()
{
}

CTransformation	CTransformation::FromPosition(CFloat3 & p)		{ return CTransformation(p,							CQuaternion(0.f, 0.f, 0.f, 1.f),	CFloat3(1.f, 1.f, 1.f));	}
CTransformation	CTransformation::FromRotation(CQuaternion & r)	{ return CTransformation(CFloat3(0.f, 0.f, 0.f),	CQuaternion(r),						CFloat3(1.f, 1.f, 1.f));	}
CTransformation	CTransformation::FromScale(CFloat3 & s)			{ return CTransformation(CFloat3(0.f, 0.f, 0.f),	CQuaternion(0.f, 0.f, 0.f, 1.f),	s);						}

CTransformation	CTransformation::FromPosition(float x, float y, float z)			{ return CTransformation(x, y, z, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f);	}
CTransformation	CTransformation::FromRotation(float x, float y, float z, float w)	{ return CTransformation(0.f, 0.f, 0.f,	x, y, z, w,	1.f, 1.f, 1.f);	}
CTransformation	CTransformation::FromScale(float x, float y, float z)				{ return CTransformation(0.f, 0.f, 0.f,	0.f, 0.f, 0.f, 1.f,	x, y, z);	}
	
CTransformation::CTransformation(const CFloat3 & p)							: Position(p), Rotation(0, 0, 0, 1), Scale(1.f, 1.f, 1.f){}
CTransformation::CTransformation(CFloat3 & p, CQuaternion & r)				: Position(p), Rotation(r), Scale(1.f, 1.f, 1.f){}
CTransformation::CTransformation(CFloat3 & p, CQuaternion & r, float s)		: Position(p), Rotation(r), Scale(s, s, s){}
CTransformation::CTransformation(CFloat3 & p, CQuaternion & r, CFloat3 & s)	: Position(p), Rotation(r), Scale(s){}
CTransformation::CTransformation(float px, float py, float pz) : Position(px, py, pz), Rotation(0.f, 0.f, 0.f, 1.f), Scale(1.f, 1.f, 1.f){}
CTransformation::CTransformation(float px, float py, float pz, float rx, float ry, float rz, float rw) : Position(px, py, pz), Rotation(rx, ry, rz, rw), Scale(1.f, 1.f, 1.f){}
CTransformation::CTransformation(float px, float py, float pz, float rx, float ry, float rz, float rw, float sx, float sy, float sz) : Position(px, py, pz), Rotation(rx, ry, rz, rw), Scale(sx, sy, sz){}
	
CTransformation::~CTransformation()
{
}
	
std::wstring CTransformation::GetTypeName()
{
	return TypeName;
}

void CTransformation::Read(CStream * s)
{
	Position.Read(s);
	Rotation.Read(s);
	Scale.Read(s);
}

int64_t CTransformation::Write(CStream * s)  
{
	int64_t n = 0;
	n += Position.Write(s);
	n += Rotation.Write(s);
	n += Scale.Write(s);
	return n;
}

void CTransformation::Write(std::wstring & s)
{
	Position.Write(s);
	s += L" ";
	s += Rotation.ToString();
	s += L" ";
	Scale.Write(s);
}

void CTransformation::Read(const std::wstring & v)
{
	auto p = CString(v).Split(L" ");

	if(p.size() != 10)
	{
		*this = Nan;
		return;
	}

	Position.x = CFloat::Parse(p[0]);
	Position.y = CFloat::Parse(p[1]);
	Position.z = CFloat::Parse(p[2]);

	Rotation.x = CFloat::Parse(p[3]);
	Rotation.y = CFloat::Parse(p[4]);
	Rotation.z = CFloat::Parse(p[5]);
	Rotation.w = CFloat::Parse(p[6]);

	Scale.x = CFloat::Parse(p[7]);
	Scale.y = CFloat::Parse(p[8]);
	Scale.z = CFloat::Parse(p[9]);
}

ISerializable * CTransformation::Clone()
{
	return new CTransformation(*this);
}
		
CString CTransformation::ToStringPRS()
{
	CString s;

	s += L"P=(";
	s += Position.ToNiceString();
	s += L")  ";

	s += L"R=(";
	s += Rotation.ToNiceString();
	s += L") ";

	s += L"S=(";
	s += Scale.ToNiceString();
	s += L") ";

	return s;
}

bool CTransformation::IsReal() const
{
	return Position.IsReal() && Rotation.IsReal() && Scale.IsReal();
}

bool CTransformation::Equals(const ISerializable & a) const
{
	return (CTransformation &)*this == (CTransformation &)a;
}
	
CTransformation CTransformation::GetInversed()
{
	//CTransformation t;
	//t.Position	= Position * -1.f;
	//t.Rotation	= Rotation.GetInversed();
	//t.Scale		= CFloat3(1.f/Scale.x, 1.f/Scale.y, 1.f/Scale.z);
	//return t;

	return CMatrix(*this).GetInversed().Decompose();
}
	
CTransformation CTransformation::operator + (const CTransformation & a)
{
	CTransformation o;
	o.Position	= Position + a.Position;
	o.Rotation	= Rotation + a.Rotation;
	o.Scale		= Scale + a.Scale;
	return o;
}

CTransformation CTransformation::operator - (const CTransformation & a)
{
	CTransformation o;
	o.Position	= Position - a.Position;
	o.Rotation	= Rotation - a.Rotation;
	o.Scale		= Scale - a.Scale;
	return o;
}

CTransformation CTransformation::operator * (const float & a)
{
	CTransformation o;
	o.Position	= Position * a;
	o.Rotation	= Rotation * a;
	o.Scale		= Scale * a;
	return o;
}
	
CTransformation CTransformation::operator * (CTransformation & a)
{
	//CTransformation t;
	//t.Position	= (CMatrix(*this) * CMatrix(a)).GetPosition();
	//t.Rotation	= Rotation * a.Rotation;
	//t.Scale		= Scale * a.Scale;
		
	CTransformation t = (CMatrix(*this) * CMatrix(a)).Decompose();
	
	return t;
}

bool CTransformation::operator == (const CTransformation & a)
{
	return	Position == a.Position &&
			Rotation == a.Rotation &&
			Scale == a.Scale;
}

bool CTransformation::operator != (const CTransformation & a)
{
	return !(*this == a);
}

		
