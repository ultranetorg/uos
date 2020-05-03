#pragma once

namespace uos
{
	class UOS_LINKING CTimer
	{
		public:
			CTimer();
			CTimer(bool isStart);
			~CTimer();

			void					Restart();
			void					Stop();
			float					GetTime();
//			void					StartInterval();
			bool					IsElapsed(float dt, bool autoreset);
			void					Pause();
			void					Resume();
		
		private:
			LARGE_INTEGER			Current;
			LARGE_INTEGER			Frequency;
			LARGE_INTEGER			StartTime;
			__int64					TotalTime;
			bool					Paused;
			bool					Running;
	};
}