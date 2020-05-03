#pragma once
#include "Float3.h"
#include "Timer.h"
#include "Animation.h"

namespace uos
{
	template<class T> class CAnimated
	{
		public:

			//virtual void Set(const T & v)
			//{
			//	AnimateTime		= 0.f;
			//	StartTime		= 0.f;
			//	Begin			= Current;
			//	End				= v;
			//	Delta			= End-Begin;
			//
			//	Current			= v;
			//	Animating		= false;
			//}
			

			//virtual void Start(const T & v, float time)
			//{
			//	Animating = true;
			//	if(Animating)
			//	{
			//		AnimateTime		= time;
			//		StartTime		= Timer.GetTime();
			//		Begin			= Current;
			//		End				= v;
			//		Delta			= End-Begin;
			//	}
			//}

			T GetNext()
			{
				if(Animation.Running)
				{
					return Begin + (End - Begin) * Animation.GetNext();
				}
				else
					return End;
			}

			//void AnimateLinearly()
			//{
			//	if(Animating)
			//	{
			//		float elapsed = Timer.GetTime() - StartTime;
			//		if(elapsed < AnimateTime)
			//		{
			//			Current = Begin + Delta * elapsed/AnimateTime;
			//		}
			//		else
			//		{
			//			Current	= End;
			//			Animating = false;
			//		}
			//	}
			//}

			//float GetLagrangeInterpolation(float xx)
			//{
			//	float li,Ln;
			//	float x[4], y[4];
			//
			//	x[0]=0;			y[0]=0;
			//	x[1]=0.334f;	y[1]=0.574f;
			//	x[2]=0.828f;	y[2]=0.962f;
			//	x[3]=1;			y[3]=1;
			//
			//	Ln=0;ÿ
			//	for(int i=0; i<4; i++)
			//	{
			//		li=1;
			//		for(int k=0; k<4; k++)
			//		{
			//			if(k!=i)
			//			{
			//				li=li*(xx-x[k])/(x[i]-x[k]);
			//			}
			//		}
			//		Ln=Ln+y[i]*li;
			//	}
			//	return Ln;
			//}

			CAnimated()
			{
			}

			CAnimated(const T & begin, const T & end, CAnimation & a)
			{
				Begin = begin;
				End = end;
				Animation = a;
			}

			CAnimation			Animation;
			T					Begin;
			T					End;
	};
	
	class UOS_LINKING CAnimatedAngles : public CAnimated<CFloat3>
	{
		public:
			CAnimatedAngles(const CFloat3 & b, const CFloat3 & e, CAnimation & a);
	};
}
