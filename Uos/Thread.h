#pragma once
#include "IType.h"
#include "Event.h"
#include "Timer.h"
#include "String.h"

namespace uos
{
	class CCore;

	class UOS_LINKING CThread : public IType
	{
		public:
			///void										Start(std::function<void()> method);

			std::function<void()>						Exited;
			std::function<void()>						Main;
			CTimer										Timer;

			CString										Name;
			CCore	*									Core;								
			HANDLE										Handle = null;

			UOS_RTTI
			CThread(CCore * e);
			~CThread();

			void										Start();
			void										Terminate();
			bool										IsRunning();
			float										GetTime();

			unsigned int static __stdcall	 			Run(void * p);
	};
}