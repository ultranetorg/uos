#include "StdAfx.h"
#include "Thread.h"
#include "Core.h"

using namespace uos;

CThread::CThread(CCore * e)
{
	Core = e;
}

CThread::~CThread()
{
	if(Handle)
	{
		CloseHandle(Handle);
		Core->UnregisterEvent(Handle);
	}
}

void CThread::Start()
{
	Timer.Restart();

	Handle	= (HANDLE)_beginthreadex(null, 0, Run, this, CREATE_SUSPENDED, null);
	Core->RegisterEvent(Handle, []{});
	ResumeThread(Handle);
}

///void CThread::Start(std::function<void()> method)
///{	
///	Method	= method;
///	///Handle	= (HANDLE)_beginthreadex(null, 0, CThread::Run, this, CREATE_SUSPENDED, null);
///
///	//Level1->RegisterEvent(Handle,	[this]()
///	//								{
///	//									Level1->UnregisterEvent(Handle);
///	//									CloseHandle(Handle);
///	//									Handle = null;
///	//									Exited();
///	//								});
///	//
///	//Timer.Start();
///	//ResumeThread(Handle);
///
///	Level1->StartThread(this,	[this]()
///								{
///									CloseHandle(Handle);
///									Handle = null;
///									Exited();
///								});
///}

void CThread::Terminate()
{
	if(IsRunning())
	{
		TerminateThread(Handle, 0);
		CloseHandle(Handle);
		Handle = null;
	}
}

bool CThread::IsRunning()
{
	return Handle != null;
}

unsigned int CThread::Run(void * p)
{
	CThread * t = (CThread *)p;
	t->Main();
	return 0;
}

float CThread::GetTime()
{
	return Timer.GetTime();
}

