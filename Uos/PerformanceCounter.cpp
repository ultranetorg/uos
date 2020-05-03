#include "StdAfx.h"
#include "PerformanceCounter.h"

using namespace uos;

CPerformanceCounter::CPerformanceCounter()
{
	LARGE_INTEGER c;
	QueryPerformanceFrequency(&c);
	Frequency = c.QuadPart;
	TotalTime	= 0;
	StartTime	= 0;
	Measures	= 0;
}

CPerformanceCounter::CPerformanceCounter(const CString & name)
{
	LARGE_INTEGER c;
	QueryPerformanceFrequency(&c);
	Frequency = c.QuadPart;
	TotalTime	= 0;
	StartTime	= 0;
	Measures	= 0;
	Name		= name;
}

CPerformanceCounter::~CPerformanceCounter()
{
}

LONGLONG CPerformanceCounter::GetCurrent()
{
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	return c.QuadPart;
}

void CPerformanceCounter::BeginMeasure()
{
	Measures++;
	StartTime = GetCurrent();
}

void CPerformanceCounter::EndMeasure()
{
	auto t = (GetCurrent() - StartTime);

	Max = max(Max, double(t)/double(Frequency));
	Min = min(Min, double(t)/double(Frequency));

	TotalTime += t;
}

double CPerformanceCounter::GetTime()
{
	return double(TotalTime)/double(Frequency);
}

int64_t CPerformanceCounter::GetMeasures()
{
	return Measures;
}

void CPerformanceCounter::Reset()
{
	TotalTime	= 0;
	Measures	= 0;
	Max = 0;
	Min = FLT_MAX;
}
