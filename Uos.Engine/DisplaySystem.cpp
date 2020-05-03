#include "StdAfx.h"
#include "DisplaySystem.h"

using namespace uos;

CDisplaySystem::CDisplaySystem(CEngineLevel * l) : CEngineEntity(l)
{
	Parameter = l->Config->Root->One(L"GraphicEngine");
		
	if(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&DXGI)) != S_OK)
	{
		throw CAttentionException(HERE, L"DXGI not found");
	}

	Verify(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2D));
	Verify(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DWrite), reinterpret_cast<IUnknown **>(&DWrite)));

	EnumerateDevices();

	DefaultDevice = Displays.Find([](auto i){ return i->Monitor->IsPrimary; });
}
	
CDisplaySystem::~CDisplaySystem()
{
	for(auto i : Displays)
	{
		delete i;
	}
	for(auto i : Adapters)
	{
		i->DxAdapter->Release();
		delete i;
	}
	for(auto i : Monitors)
	{
		delete i;
	}

	D2D->Release();
	DXGI->Release();
}

void CDisplaySystem::EnumerateDevices()
{

	UINT i = 0;
	IDXGIAdapter * a;

	auto shcore = LoadLibrary(L"Shcore.dll");

	while(DXGI->EnumAdapters(i, &a) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		a->GetDesc(&desc);

		IDXGIAdapter1 * a1;
		a->QueryInterface(__uuidof(IDXGIAdapter1), (void **)&a1);

		auto da = new CDisplayAdapter();
		da->DxAdapter = a1;
		da->Luid = desc.AdapterLuid;
		da->Name = desc.Description;
		da->MemorySize = desc.DedicatedVideoMemory;

		Adapters.push_back(da);

		UINT j = 0;
		IDXGIOutput * o;
		
		while(a->EnumOutputs(j, &o) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC od;
			o->GetDesc(&od);

			auto dd = new CDisplayDevice();

			dd->Name = od.DeviceName;
			dd->Adapter = da;
			dd->Monitor = GetMonitor(od.Monitor);
			dd->NRect = dd->Monitor->GetNRect();
			dd->NRectWork = dd->Monitor->GetNRectWork();

			DEVMODE dm = { 0 };
			dm.dmSize = sizeof(dm);
			EnumDisplaySettings(od.DeviceName, ENUM_CURRENT_SETTINGS, &dm);

			dd->Resolution = {float(dm.dmPelsWidth), float(dm.dmPelsHeight), 0};

			if(!shcore)
			{
				auto screen = GetDC(NULL);
				dd->Dpi = {float(GetDeviceCaps(screen, LOGPIXELSX)), float(GetDeviceCaps(screen, LOGPIXELSY))};
				ReleaseDC(NULL, screen);
				dd->Scaling = {dd->Dpi.x/96, dd->Dpi.y/96};
			} 
			else
			{
				auto f = (HRESULT (*)(HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *))GetProcAddress(shcore, "GetDpiForMonitor");
			
				UINT x, y;
				f(dd->Monitor->Handle, MDT_DEFAULT, &x, &y);
			
				dd->Dpi = {float(x), float(y)};
				dd->Scaling = {dd->Dpi.x/96, dd->Dpi.y/96};
			}

			Scaling.x = max(dd->Scaling.x, Scaling.x);
			Scaling.y = max(dd->Scaling.y, Scaling.y);


			//dd->NRectWork.X			*= dd->Scaling.x;
			//dd->NRectWork.Y			*= dd->Scaling.y;
			//dd->NRectWork.Width		*= dd->Scaling.x;
			//dd->NRectWork.Height	*= dd->Scaling.y;


			Displays.push_back(dd);
			++j;
			o->Release();
		}

		++i;
		a->Release();
	}

	if(shcore)
	{
		FreeLibrary(shcore);
	}

	for(auto i : Adapters)
	{
		Level->Log->ReportMessage(this, L"Adapter: %p %s %I64dMB Id=%08X%08X", i, i->Name, i->MemorySize / (1024 * 1024), i->Luid.HighPart, i->Luid.LowPart);
	}

	for(auto i : Monitors)
	{
		Level->Log->ReportMessage(this, L"Monitor: %p %s {%s}", i, i->Name, CGdiRect(i->GetNRect()).ToString());
	}

	for(auto i : Displays)
	{
		Level->Log->ReportMessage(this, L"Device: %p %-15s %-20s   %-20s   %I64dMB   {%s}    Id=%08X%08X",	i,
																											i->Name,
																											i->Monitor->Name,
																											i->Adapter->Name,
																											i->Adapter->MemorySize / (1024 * 1024),
																											i->NRect.ToString(),
																											i->Adapter->Luid.HighPart, i->Adapter->Luid.LowPart);
	}
}

CDisplayMonitor * CDisplaySystem::GetMonitor(HMONITOR h)
{
	for(auto i : Monitors)
	{
		if(i->Handle == h)
		{
			return i;
		}
	}

	CDisplayMonitor * dm = new CDisplayMonitor(h);
	Monitors.push_back(dm);
	return dm;
}

/*
void CGraphicEngine::GetDisplayAdapterInfo(const CString & ddName, __int64 * memorySize, CString * name)
{
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

	for(int i=0; i<24; i++)
	{
		if(EnumDisplayDevices(null, i, &dd, 0))
		{
			if(dd.StateFlags & (DISPLAY_DEVICE_ACTIVE | DISPLAY_DEVICE_ATTACHED))
			{
				if(ddName == dd.DeviceName)
				{
					*name		= dd.DeviceString;
					*memorySize	= 0;

					HKEY dev;
					DWORD size=8;
					if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, &dd.DeviceKey[18], 0, KEY_READ, &dev) == ERROR_SUCCESS)
					{ 
						RegQueryValueEx(dev, L"HardwareInformation.MemorySize", null, null, (BYTE *)memorySize, &size);

						RegCloseKey(dev);
					}
					return;
				}
			}
		}
		else
		{
			break;
		}
	}
}*/

CDisplayDevice * CDisplaySystem::GetDisplayDevice(const CString & ddName)
{
	for(int i=0; i<Displays.Count(); i++)
	{
		if(CInt32::ToString(i) == ddName)
		{
			return Displays[i];
		}

		if(Displays[i]->Name == ddName)
		{
			return Displays[i];
		}
	}
	return null;
}

CDisplayDevice * CDisplaySystem::GetDisplayDevice(CGdiRect & nr)
{
	for(auto i : Displays)
	{
		CGdiRect ir = nr.Intersect(i->NRect);
		if(ir.Width * ir.Height > (nr.Width * nr.Height)/2) // большая часть nr лежит в (*i)->NRect
		{
			return i;
		}
	}
	return Displays.front();
}

CArray<CDisplayDevice *> CDisplaySystem::GetDisplayDevices(CDisplayAdapter * a)
{
	CArray<CDisplayDevice *> devices;
	for(auto i : Displays)
	{
		if(i->Adapter == a)
		{
			devices.push_back(i);
		}
	}
	return devices;
}

CDisplayDevice * CDisplaySystem::GetDefaultDevice()
{
	return DefaultDevice;
}	

CDisplayDevice * CDisplaySystem::GetDisplayDevice(HMONITOR m)
{
	for(auto i : Displays)
	{
		if(i->Monitor->Handle == m)
		{
			return i;
		}
	}
	return null;
}
