#include "stdafx.h"
#include "Timer.h"

using namespace uos;

CTimer::CTimer()
{
	QueryPerformanceFrequency(&Frequency);
	Running	= false;
	Paused	= false;
}

CTimer::CTimer(bool isStart)
{
	QueryPerformanceFrequency(&Frequency);
	Running	= false;
	Paused = false;
	
	if(isStart)
	{
		Restart();
	}
}

CTimer::~CTimer()
{
}

void CTimer::Restart()
{
	TotalTime = 0;
	Paused	= false;
	Running	= true;
	QueryPerformanceCounter(&StartTime);
}

void CTimer::Stop()
{
	TotalTime = 0;
	Paused	= false;
	Running	= false;
}

void CTimer::Pause()
{
	if(Running && !Paused)
	{
		QueryPerformanceCounter(&Current);
		TotalTime += (Current.QuadPart - StartTime.QuadPart);
		Paused = true;
	}
}

void CTimer::Resume()
{
	if(Running && Paused)
	{
		QueryPerformanceCounter(&StartTime);
		Paused = false;
	}
	else
	{
		Restart();
	}
}

float CTimer::GetTime()
{
	if(Running)
	{
		if(Paused)
		{
			return float(TotalTime)/float(Frequency.QuadPart);
		}
		else
		{
			QueryPerformanceCounter(&Current);
			return float(TotalTime + (Current.QuadPart - StartTime.QuadPart)) / float(Frequency.QuadPart);
		}
	} 
	else
	{
		return 0.f;
	}
}

bool CTimer::IsElapsed(float dt, bool autoreset)
{
	//QueryPerformanceCounter(&Current);
	
	if(GetTime() > dt)
	{
		if(autoreset)
		{
			Restart();
		}
		return true;
	}
	return false;
}
