#include "stdafx.h"
#include "GdiRect.h"

using namespace uos;

const std::wstring CGdiRect::TypeName = L"int32.rect";

CGdiRect CGdiRect::operator+(const CGdiRect & a)
{
	CGdiRect r;
	r.X = min(X, a.X);
	r.Y = min(Y, a.Y);
	r.Width = max(GetRight(), a.GetRight()) - r.X;
	r.Height = max(GetButtom(), a.GetButtom()) - r.Y;
	return r;
}

bool CGdiRect::operator!=(const CGdiRect & a) const
{
	return !(*this == a);
}

bool CGdiRect::operator==(const CGdiRect & a) const
{
	return a.X == X && a.Y == Y && a.Width == Width && a.Height == Height;
}

std::wstring CGdiRect::GetTypeName()
{
	return TypeName;
}

void CGdiRect::Read(CStream * s)
{
	s->Read(&X, 4 * 4);
}

void CGdiRect::Write(std::wstring & s)
{
	s += CString::Format(L"%d %d %d %d", X, Y, Width, Height);
}

int64_t CGdiRect::Write(CStream * s)
{
	throw CException(HERE, L"Not implemented");
}

void CGdiRect::Read(const std::wstring & v)
{
	auto parts = CString(v).Split(L" ");
	X = CInt32::Parse(parts[0]);
	Y = CInt32::Parse(parts[1]);
	Width = CInt32::Parse(parts[2]);
	Height = CInt32::Parse(parts[3]);
}

ISerializable * CGdiRect::Clone()
{
	return new CGdiRect(X, Y, Width, Height);
}

bool CGdiRect::Equals(const ISerializable & a) const
{
	return X == ((CGdiRect &)a).X && Y == ((CGdiRect &)a).Y && Width == ((CGdiRect &)a).Width && Height == ((CGdiRect &)a).Height;
}

CGdiRect::CGdiRect(const CBuffer & b)
{
	if(b.GetSize() != sizeof(CGdiRect))
	{
		throw CException(HERE, L"Wrong size");
	}

	*this = *((CGdiRect *)((CBuffer &)b).GetData());
}

CGdiRect::CGdiRect()
{

}

CGdiRect::CGdiRect(int x, int y, int w, int h)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
}

CGdiRect::CGdiRect(const RECT & r)
{
	X = int(r.left);
	Y = int(r.top);
	Width = int(r.right - r.left);
	Height = int(r.bottom - r.top);
}

void CGdiRect::Set(int x, int y, int w, int h)
{
	X = x;
	Y = y;
	Width = w;
	Height = h;
}

void CGdiRect::MakeEmpty()
{
	X = 0;
	Y = 0;
	Width = 0;
	Height = 0;
}

CGdiRect CGdiRect::GetLocal()
{
	CGdiRect r = *this;
	r.X = 0;
	r.Y = 0;
	return r;
}

POINT CGdiRect::GetCenter()
{
	POINT p = {X + Width / 2, Y + Height / 2};
	return p;
}

bool CGdiRect::IsEmpty()
{
	return Width <= 0 || Height <= 0;
}

CGdiRect CGdiRect::GetInflated(int dx, int dy)
{
	CGdiRect r;
	r.X = X - dx;
	r.Y = Y - dy;
	r.Width = Width + dx * 2;
	r.Height = Height + dy * 2;
	return r;
}

bool CGdiRect::IsGreaterOrEqual(int w, int h)
{
	return Width >= w && Height >= h;
}

bool CGdiRect::IsIntersectWOB(const CGdiRect & a)
{
	if((a.X < X && X < a.X + a.Width) && (a.Y < Y && Y < a.Y + a.Height))
		return true;

	if((a.X < X + Width && X + Width < a.X + a.Width) && (a.Y < Y && Y < a.Y + a.Height))
		return true;

	if((a.X < X && X < a.X + a.Width) && (a.Y < Y + Height && Y + Height < a.Y + a.Height))
		return true;

	if((a.X < X + Width && X + Width < a.X + a.Width) && (a.Y < Y + Height && Y + Height < a.Y + a.Height))
		return true;

	return false;
}

bool CGdiRect::IsIntersect(const CGdiRect & a)
{
	if((a.X <= X && X < a.X + a.Width) && (a.Y <= Y && Y < a.Y + a.Height))
		return true;

	if((a.X < X + Width && X + Width < a.X + a.Width) && (a.Y <= Y && Y < a.Y + a.Height))
		return true;

	if((a.X <= X && X < a.X + a.Width) && (a.Y < Y + Height && Y + Height < a.Y + a.Height))
		return true;

	if((a.X < X + Width && X + Width < a.X + a.Width) && (a.Y < Y + Height && Y + Height < a.Y + a.Height))
		return true;

	return false;
}

CGdiRect CGdiRect::Intersect(const CGdiRect & a)
{
	int l = 0, r = 0, t = 0, b = 0;

	if(a.X == X)
		l = X;
	if(a.X + a.Width == X + Width)
		r = X + Width;
	if(a.X < X && X < a.X + a.Width)
		l = X;
	if(a.X < X + Width && X + Width < a.X + a.Width)
		r = X + Width;
	if(X < a.X && a.X < X + Width)
		l = a.X;
	if(X < a.X + a.Width && a.X + a.Width < X + Width)
		r = a.X + a.Width;

	if(a.Y == Y)
		t = Y;
	if(a.Y + a.Height == Y + Height)
		b = Y + Height;
	if(a.Y < Y && Y < a.Y + a.Height)
		t = Y;
	if(a.Y < Y + Height && Y + Height < a.Y + a.Height)
		b = Y + Height;
	if(Y < a.Y && a.Y < Y + Height)
		t = a.Y;
	if(Y < a.Y + a.Height && a.Y + a.Height < Y + Height)
		b = a.Y + a.Height;

	return CGdiRect(l, t, r - l, b - t);
}

int CGdiRect::GetArea()
{
	return Width * Height;
}

int CGdiRect::GetWidth()
{
	return Width;
}

int CGdiRect::GetHeight()
{
	return Height;
}

int CGdiRect::GetRight() const
{
	return X + Width;
}

int CGdiRect::GetButtom() const
{
	return Y + Height;
}

bool CGdiRect::Contain(int x, int y)
{
	return (X <= x && x < X + Width) && (Y <= y && y < Y + Height);
}

bool CGdiRect::ContainWOB(int x, int y)
{
	return	(X < x && x < X + Width) && (Y < y && y < Y + Height);
}

RECT CGdiRect::GetAsRECT()
{
	RECT r;
	SetRect(&r, (int)X, (int)Y, (int)GetRight(), (int)GetButtom());
	return r;
}

CString CGdiRect::ToString()
{
	return CString::Format(L"%d %d %d %d", X, Y, Width, Height);
}
