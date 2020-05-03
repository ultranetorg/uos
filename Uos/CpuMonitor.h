#pragma once
#include "Timer.h"

namespace uos
{
	class UOS_LINKING CCpuMonitor
	{
		public:
			float							GetUsage();
			
			CCpuMonitor();
			~CCpuMonitor();

		private:
			ULARGE_INTEGER					CreationTime;
			ULARGE_INTEGER					ExitTime;

			float							CpuUsage;
			CTimer							CpuTimer;

			ULARGE_INTEGER					UserTimeBegin;
			ULARGE_INTEGER					UserTimeEnd;
			ULARGE_INTEGER					KernelTimeBegin;
			ULARGE_INTEGER					KernelTimeEnd;

			ULARGE_INTEGER					SystemIdleTimeBegin;
			ULARGE_INTEGER					SystemIdleTimeEnd;
			ULARGE_INTEGER					SystemUserTimeBegin;
			ULARGE_INTEGER					SystemUserTimeEnd;
			ULARGE_INTEGER					SystemKernelTimeBegin;
			ULARGE_INTEGER					SystemKernelTimeEnd;
	};
}