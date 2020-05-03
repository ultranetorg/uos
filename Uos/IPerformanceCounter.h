#pragma once

namespace uos
{
	class IPerformanceCounter
	{
		public:
			virtual void								BeginMeasure()=0;
			virtual void								EndMeasure()=0;

			virtual ~IPerformanceCounter(){}
	};
}