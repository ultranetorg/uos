#include "StdAfx.h"
#include "Rect.h"

using namespace uos;

CRect::CRect()
{
}

CRect::CRect(std::initializer_list<float> l)
{
	auto i = l.begin();
	X = i[0];
	Y = i[1];
	W = i[2];
	H = i[3];
}

CRect::CRect(const RECT & r, const CFloat2 o)
{
	X = float(r.left - o.x);
	Y = float(o.y - r.bottom);
	W = float(r.right - r.left);
	H = float(r.bottom - r.top);
}

CRect::CRect(const CGdiRect & r, const CFloat2 o)
{
	X = r.X - o.x;
	Y = o.y - r.GetButtom();
	W = (float)r.Width;
	H = (float)r.Height;
}

CRect::CRect(float x, float y, float w, float h)
{
	X = x;
	Y = y;
	W = w;
	H = h;
}

CRect::CRect(CFloat2 & p, CFloat2 & s)
{
	X = p.x;
	Y = p.y;
	W = s.x;
	H = s.y;
}

bool CRect::Equals(const ISerializable & a) const
{
	return	X == ((CRect &)a).X &&
			Y == ((CRect &)a).Y &&
			W == ((CRect &)a).W &&
			H == ((CRect &)a).H ;
}

RECT CRect::ToRECT(CSize & s)
{
	double l = X;
	double t = s.H - (Y + H);
	double r = X + W;
	double b = s.H - Y;

	auto e = double(LONG_MAX - 1); 

	t = CFloat::Clamp(t, -e, e);
	l = CFloat::Clamp(l, -e, e);
	r = CFloat::Clamp(r, -e, e);
	b = CFloat::Clamp(b, -e, e);
										  
	return {LONG(l), LONG(t), LONG(r), LONG(b)};
}

void CRect::Set(float x, float y, float w, float h)
{
	X = x;
	Y = y;
	W = w;
	H = h;
}

void CRect::MakeEmpty()
{
	X = 0;
	Y = 0;
	W = 0;
	H = 0;
}

CRect CRect::GetLocal()
{
	CRect r = *this;
	r.X = 0;
	r.Y = 0;
	return r;
}

CFloat2 CRect::GetCenter()
{
	return CFloat2(float(X + W / 2), float(Y + H / 2));
}

bool CRect::IsEmpty()
{
	return W <= 0 || H <= 0;
}

CRect CRect::GetInflated(float dx, float dy)
{
	CRect r;
	r.X = X - dx;
	r.Y = Y - dy;
	r.W = W + dx * 2;
	r.H = H + dy * 2;
	return r;
}

bool CRect::IsGreaterOrEqual(float w, float h)
{
	return W >= w && H >= h;
}

bool CRect::IsIntersectWOB(const CRect & a)
{
	if((a.X < X && X < a.X + a.W) && (a.Y < Y && Y < a.Y + a.H))
		return true;

	if((a.X < X + W && X + W < a.X + a.W) && (a.Y < Y && Y < a.Y + a.H))
		return true;

	if((a.X < X && X < a.X + a.W) && (a.Y < Y + H && Y + H < a.Y + a.H))
		return true;

	if((a.X < X + W && X + W < a.X + a.W) && (a.Y < Y + H && Y + H < a.Y + a.H))
		return true;

	return false;
}


bool CRect::Intersects(const CRect & a)
{
	if(a.X == X);
	else if(a.X < X && X < a.X + a.W);
	else if(X < a.X && a.X < X + W);
	else 
		return false;
	
	if(a.X + a.W == X + W);
	else if(a.X < X + W && X + W < a.X + a.W);
	else if(X < a.X + a.W && a.X + a.W < X + W);
	else
		return false;

	if(a.Y == Y);
	else if(a.Y < Y && Y < a.Y + a.H);
	else if(Y < a.Y && a.Y < Y + H);
	else
		return false;
	
	if(a.Y + a.H == Y + H);
	else if(a.Y < Y + H && Y + H < a.Y + a.H);
	else if(Y < a.Y + a.H && a.Y + a.H < Y + H);
	else
		return false;

	return true;
}


CRect CRect::Intersect(const CRect & a)
{
	float l = 0, r = 0, t = 0, b = 0;

	if(a.X == X)
		l = X;
	else if(a.X < X && X < a.X + a.W)
		l = X;
	else if(X < a.X && a.X < X + W)
		l = a.X;
	
	if(a.X + a.W == X + W)
		r = X + W;
	else if(a.X < X + W && X + W < a.X + a.W)
		r = X + W;
	else if(X < a.X + a.W && a.X + a.W < X + W)
		r = a.X + a.W;

	if(a.Y == Y)
		b = Y;
	else if(a.Y < Y && Y < a.Y + a.H)
		b = Y;
	else if(Y < a.Y && a.Y < Y + H)
		b = a.Y;
	
	if(a.Y + a.H == Y + H)
		t = Y + H;
	else if(a.Y < Y + H && Y + H < a.Y + a.H)
		t = Y + H;
	else if(Y < a.Y + a.H && a.Y + a.H < Y + H)
		t = a.Y + a.H;

	return CRect(l, b, r - l, t - b);
}

void CRect::SetPosition(CFloat2 & v)
{
	X = (float)v.x;
	Y = (float)v.y;
}

float CRect::GetArea()
{
	return W*H;
}

float CRect::GetWidth()
{
	return W;
}

float CRect::GetHeight()
{
	return H;
}

CFloat2 CRect::GetLT()
{
	return CFloat2(GetLeft(), GetTop());
}

CFloat2 CRect::GetLB()
{
	return CFloat2(GetLeft(), GetBottom());
}

CFloat2 CRect::GetRT()
{
	return CFloat2(GetRight(), GetTop());
}

CFloat2 CRect::GetRB()
{
	return CFloat2(GetRight(), GetBottom());
}

float CRect::GetRight() const
{
	return X + W;
}

float CRect::GetBottom() const
{
	return Y;
}

float CRect::GetTop() const
{
	return Y + H;
}

float CRect::GetLeft() const
{
	return X;
}

CFloat2 CRect::GetPoint(float kw, float kh)
{
	CFloat2 p;
	p.x = float(X) + float(W)*kw;
	p.y = float(Y) + float(H)*kh;
	return p;
}

void CRect::Bound(CFloat2 & v)
{
	if(v.x < (float)GetRight())
	{
		v.x = (float)GetRight();
	}
	if(v.x > (float)GetLeft())
	{
		v.x = (float)GetLeft();
	}
	if(v.y < (float)GetBottom())
	{
		v.y = (float)GetBottom();
	}
	if(v.y > (float)GetTop())
	{
		v.y = (float)GetTop();
	}
}

bool CRect::Contains(float x, float y)
{
	return (X <= x && x < X + W) && (Y <= y && y < Y + H);
}

bool CRect::ContainNoBorder(float x, float y)
{
	return	(X < x && x < X + W) && (Y < y && y < Y + H);
}

CRect CRect::operator+(const CRect & a)
{
	CRect r;
	r.X = min(X, a.X);
	r.Y = min(Y, a.Y);
	r.W = max(GetRight(), a.GetRight()) - r.X;
	r.H = max(GetBottom(), a.GetBottom()) - r.Y;
	return r;
}

bool CRect::operator!=(const CRect & a) const
{
	return !(*this == a);
}

bool CRect::operator==(const CRect & a) const
{
	return a.X == X && a.Y == Y && a.W == W && a.H == H;
}

bool CRect::ContainGDI(float x, float y)
{
	return (X <= x && x < X + W) && (Y < y && y <= Y + H);
}

bool CRect::Contains(CFloat2 & p)
{
	return Contains(p.x, p.y);
}
std::wstring CRect::GetTypeName()
{
	return L"float32.rect";
}

void CRect::Read(CStream * s)
{
	throw CException(HERE, L"Not implemented");
}

int64_t CRect::Write(CStream * s)  
{
	throw CException(HERE, L"Not implemented");
}

void CRect::Write(std::wstring & s)
{
	s += CString::Format(L"%g %g %g %g", X, Y, W, H);
}

void CRect::Read(const std::wstring & v)
{
	auto parts = CString(v).Split(L" ");
	X = CFloat::Parse(parts[0]);
	Y = CFloat::Parse(parts[1]);
	W = CFloat::Parse(parts[2]);
	H = CFloat::Parse(parts[3]);
}

ISerializable * CRect::Clone()
{
	return new CRect(X, Y, W, H);
}
