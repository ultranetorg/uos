#include "StdAfx.h"
#include "DateTime.h"

using namespace uos;

const CDateTime	CDateTime::Min = CDateTime(0);
const CString	CDateTime::DefaultFormat = L"%F %T";

CDateTime::CDateTime()
{
}

CDateTime::CDateTime(time_t t)
{
	Value = t;
}

CDateTime::CDateTime(int year, int mon, int day, int hour, int minute, int second, int zone)
{
	tm a = {};
	a.tm_year = year - 1900;
	a.tm_mon = mon;
	a.tm_mday = day;
	a.tm_hour = hour;
	a.tm_min = minute;
	a.tm_sec = second;

	Value = _mkgmtime(&a) - zone * 60;
}

CDateTime CDateTime::Now()
{
	_timeb b;
	_ftime_s(&b);
	return CDateTime(b.time);
}

CDateTime CDateTime::UtcNow()
{
	_timeb b;
	_ftime_s(&b);

	return CDateTime(b.time + b.timezone / 60);
}

CDateTime CDateTime::Parse(CString const & t, CString const & f)
{
	std::tm tm = {};
	std::wistringstream iss(t);
	//ss.imbue(std::locale("en_US.utf-8"));
	iss >> std::get_time(&tm, f.data());

	return CDateTime(_mkgmtime(&tm));
}

CString CDateTime::ToString(CString const & f)
{
	std::tm tm = {};
	gmtime_s(&tm, &Value);

	std::wostringstream oss;
	oss << std::put_time(&tm, f.data());
	return oss.str();
}

CString CDateTime::ToString()
{
	return ToString(DefaultFormat);
}

std::wstring CDateTime::GetTypeName()
{
	return L"datetime";
}

void CDateTime::Read(CStream * s)
{
	s->Read(&Value, 8);
}

int64_t CDateTime::Write(CStream * s)  
{
	return s->Write(&Value, 8);
}

void CDateTime::Write(std::wstring & s)
{
	s += ToString();
}

void CDateTime::Read(const std::wstring & b)
{
	*this = Parse(b, L"%Y-%m-%d %H:%M:%S");
}

ISerializable * CDateTime::Clone()
{
	return new CDateTime(Value);
}

bool CDateTime::Equals(const ISerializable & a) const
{
	return Value == ((CDateTime &)a).Value;
}

double CDateTime::operator-(const CDateTime & a)
{
	return difftime(Value, a.Value);
}

void CDateTime::SetValue(CString const & v)
{
	Parse(v, DefaultFormat);
}

CString CDateTime::GetValue()
{
	return ToString();
}

int CDateTime::Compare(IComparable * v)
{
	if(Value - ((CDateTime *)v)->Value < 0)
		return -1;

	if(Value - ((CDateTime *)v)->Value > 0)
		return 1;

	return 0;
}
