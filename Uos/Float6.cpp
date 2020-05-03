#include "stdafx.h"
#include "Float6.h"

using namespace uos;

const CFloat6		CFloat6::Nan = CFloat6(NAN, NAN, NAN, NAN, NAN, NAN);
const std::wstring	CFloat6::TypeName = L"float32.vector6";

void CFloat6::Write(std::wstring & s)
{
	s += ToString();
}

//CFloat6 & CFloat6::operator=(float a)
//{
//	LF = a; RT = a; BM = a; TP = a; FT = a; BK = a;
//	return *this;
//}

CFloat6::CFloat6(float lf, float rt, float bm, float tp, float ft, float bk) : LF(lf), RT(rt), BM(bm), TP(tp), FT(ft), BK(bk)
{
}

CFloat6::CFloat6()
{
}

CFloat6::CFloat6(float a)
{
	LF = a; RT = a; BM = a; TP = a; FT = a; BK = a;
}

CString CFloat6::ToString()
{
	return CString::Format(L"%g %g %g %g %g %g", LF, RT, BM, TP, FT, BK);
}

std::wstring CFloat6::GetTypeName()
{
	return TypeName;
}

void CFloat6::Read(CStream * s)
{
	s->Read(&LF, 6 * 4);
}

int64_t CFloat6::Write(CStream * s)
{
	return s->Write(&LF, 6 * 4);
}

void CFloat6::Read(const std::wstring & v)
{
	auto p = v.data();

	float * f[] = { &LF, &RT, &BM, &TP, &FT, &BK };

	int n = 0;

	for(auto i : f)
	{
		swscanf_s(p, L"%f", i);	

		while(*p != L' ' && *p != 0)
			p++;

		n++;

		if(*p == 0)
			break;

		while(*p == L' ')
			p++;
	}

	if(n == 1)
	{
		RT = BM = TP = FT = BK = (LF);
	}
}

ISerializable * CFloat6::Clone()
{
	return new CFloat6(*this);
}

bool CFloat6::Equals(const ISerializable & a) const
{
	auto & b = (CFloat6 &)a;
	return b.LF == LF && b.RT == RT && b.BM == BM && b.TP == TP && b.FT == FT && b.BK == BK;
}
