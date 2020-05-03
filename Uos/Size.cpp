#include "stdafx.h"
#include "Size.h"
#include "Converter.h"

using namespace uos;

CSize				CSize::Nan = CSize(NAN, NAN, NAN);	
CSize				CSize::Empty = CSize(0, 0, 0);
CSize				CSize::Max = CSize(FLT_MAX, FLT_MAX, FLT_MAX);	
const std::wstring	CSize::TypeName = L"float32.size";

CSize::CSize()
{
	//W = NAN;
	//H = NAN;
	//D = NAN;
}

CSize::CSize(float w, float h, float d)
{
	W = w;
	H = h;
	D = d;
}

std::wstring CSize::GetTypeName()
{
	return TypeName;
}

void CSize::Read(CStream * s)
{
	s->Read(&W, 3 * 4);
}

int64_t CSize::Write(CStream * s)  
{
	return s->Write(&W, 3 * sizeof(W));
}

void CSize::Write(std::wstring & s)
{
	s += CString::Format(L"%g %g %g", W, H, D);
}

void CSize::Read(const std::wstring & v)
{
	if(v.empty())
	{
		*this = Nan;
		return;
	}

	auto p = v.data();

	W	= CFloat::Parse(p);
	H	= CFloat::Parse(p + v.find(L' '));
	D	= CFloat::Parse(p + v.find(L' ', v.find(L' ') + 1));
}

ISerializable * CSize::Clone()
{
	return new CSize(*this);
}
	
bool CSize::operator != (const CSize & a) const
{
	return !(*this == a);
}

bool CSize::operator == (const CSize & a) const
{
	return	std::isfinite(W) && std::isfinite(a.W) && W == a.W &&
			std::isfinite(H) && std::isfinite(a.H) && H == a.H &&
			std::isfinite(D) && std::isfinite(a.D) && D == a.D;
}

bool CSize::Equals(const ISerializable & a) const
{
	return W == ((CSize &)a).W && H == ((CSize &)a).H && D != ((CSize &)a).D;
}


CString CSize::ToNiceString()
{
	return CString::Format(L"%g %g %g", W, H, D);
}

bool CSize::IsReal() const
{
	return isfinite(W) && isfinite(H) && isfinite(D);
}

bool CSize::IsEmpty() const
{
	return *this == Empty;
}

CSize CSize::operator * (float a)
{
	return CSize(W * a, H * a, D * a);
}

CSize::operator bool()  const
{
	return IsReal();
}