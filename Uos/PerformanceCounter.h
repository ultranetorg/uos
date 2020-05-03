#pragma once
#include "IPerformanceCounter.h"
#include "String.h"

namespace uos
{
	class UOS_LINKING CPerformanceCounter : public IPerformanceCounter
	{
		public:
			CString										Name;

			double										Min;
			double										Max;

			void										Reset();
			int64_t										GetMeasures();
			void										BeginMeasure();
			void										EndMeasure();
			double										GetTime();

			CPerformanceCounter();
			CPerformanceCounter(const CString & name);
			~CPerformanceCounter();

		private:
			LONGLONG									Frequency;
			LONGLONG									StartTime;
			LONGLONG									TotalTime;
			LONGLONG									Measures;

			LONGLONG									GetCurrent();
	};
}