#include "StdAfx.h"
#include "CpuMonitor.h"

using namespace uos;

CCpuMonitor::CCpuMonitor()
{
	CpuUsage = 0.f;
	CpuTimer.Restart();
}

CCpuMonitor::~CCpuMonitor()
{
}

float CCpuMonitor::GetUsage()
{
	if(CpuTimer.IsElapsed(1.f, true))
	{
		GetSystemTimes((FILETIME *)(&SystemIdleTimeEnd), (FILETIME *)(&SystemKernelTimeEnd), (FILETIME *)(&SystemUserTimeEnd));
		GetProcessTimes(::GetCurrentProcess(), (FILETIME *)(&CreationTime), (FILETIME *)(&ExitTime),  (FILETIME *)(&KernelTimeEnd), (FILETIME *)(&UserTimeEnd));

		float cpu =  float((KernelTimeEnd.QuadPart - KernelTimeBegin.QuadPart) + (UserTimeEnd.QuadPart - UserTimeBegin.QuadPart))/float((SystemKernelTimeEnd.QuadPart - SystemKernelTimeBegin.QuadPart) + (SystemUserTimeEnd.QuadPart - SystemUserTimeBegin.QuadPart))*100.0f;

		UserTimeBegin			= UserTimeEnd;
		KernelTimeBegin			= KernelTimeEnd;
		SystemIdleTimeBegin		= SystemIdleTimeEnd;
		SystemUserTimeBegin		= SystemUserTimeEnd;
		SystemKernelTimeBegin	= SystemKernelTimeEnd;

		CpuUsage = cpu;
	}

	return CpuUsage;
}
