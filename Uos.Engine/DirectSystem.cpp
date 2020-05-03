#include "StdAfx.h"
#include "DirectSystem.h"

using namespace uos;

CDirectSystem::CDirectSystem(CEngineLevel * l) : CDisplaySystem(l)
{
	Parameter = l->Config->Root->One(L"GraphicEngine");
	
	PcUpdate = new CPerformanceCounter(L"GraphicEngine update");
	Level->Core->AddPerformanceCounter(PcUpdate);
}
	
CDirectSystem::~CDirectSystem()
{
	for(auto i : Devices)
	{
		delete i;
	}

	delete PcUpdate;
}

CDirectDevice * CDirectSystem::GetAppropriateDevice(CDisplayDevice * dd)
{
	//CDirectDevice * d = new CDirectDevice(Level, this, dd);
	//d->Index = Devices.size();
	//Devices.push_back(d);
	//return d->AddTarget();
	throw CException(HERE, L"Not implemented");
}

CDirectDevice * CDirectSystem::GetAppropriateDevice(CWindowScreen * w)
{
	auto dd = GetDisplayDevice(w->GetRect());
	if(dd == null)
	{
		CGdiRect r = w->GetRect();
		throw CAttentionException(HERE, L"Nearest display not found, window: x=%d y=%d w=%d h=%d", r.X, r.Y, r.Width, r.Height);
	}

	CDirectDevice * d = null;

	auto strategy = Parameter->One(L"DeviceCreationStrategy")->Get<CString>();

	if(strategy == L"ByDevice" || strategy == L"Auto")
	{
		for(auto i : Devices)
		{
			if(i->Display == dd)
			{
				return i;
			}
		}
	}
	else if(strategy == L"ByAdapter")
	{
		for(auto i : Devices)
		{
			if(i->Display->Adapter == dd->Adapter)
			{
				return i;
			}
		}
	}

	d = new CDirectDevice(Level, this, dd);
	Devices.push_back(d);

	return d;
}
