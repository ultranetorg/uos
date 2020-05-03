#pragma once
#include "Timer.h"
#include "Xon.h"

namespace uos
{	
	class CAnimation
	{
		public:
			bool				Running = false;
			CTimer				Timer;
			float				Time = NAN;

			float GetNext()
			{
				if(Running && Timer.GetTime() < Time)
				{
					//Current = Begin + (End-Begin) * GetLagrangeInterpolation(elapsed/AnimateTime);
					//return  sqrt(Timer.GetTime()/Time);
					return  pow(Timer.GetTime()/Time - 1, 3) + 1; // ((x-1)^3)+1
				}
				else
				{
					Running = false;
					return 1.f;
				}
			}

			CAnimation()
			{
			}

			CAnimation & operator = (const CAnimation & a)
			{
				Running = true;
				Time = a.Time;
				Timer.Restart();

				return *this;
			}

			CAnimation(const CAnimation & a)
			{
				Running = true;
				Time = a.Time;
				Timer.Restart();
			}

			CAnimation(float t)
			{
				Running	= true;
				Time		= t;
				Timer.Restart();
			}

			CAnimation(CXon * on)
			{
				Running	= true;
				Time		= on->Get<CFloat>(L"Time");
				Timer.Restart();
			}

			auto IsReal()
			{
				return isfinite(Time);
			}
	};
}