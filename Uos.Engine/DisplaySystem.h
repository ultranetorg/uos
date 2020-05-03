#pragma once
#include "Display.h"
#include "WindowScreen.h"

namespace uos
{
	class CDisplaySystem : public CEngineEntity
	{
		public:
			CArray<CDisplayMonitor *>					Monitors;
			CArray<CDisplayAdapter *>					Adapters;
			CArray<CDisplayDevice *>					Displays;
			IDXGIFactory1 *								DXGI;
			ID2D1Factory *								D2D;
			IDWriteFactory *							DWrite;

			CXon *										Parameter;
			CDisplayDevice *							DefaultDevice;

			CFloat2										Scaling = {FLT_MIN, FLT_MIN};

			UOS_RTTI
			CDisplaySystem(CEngineLevel * e);
			virtual ~CDisplaySystem();
	
			CDisplayDevice *							GetDefaultDevice();
			CDisplayDevice *							GetDisplayDevice(const CString & m);
			CDisplayDevice *							GetDisplayDevice(HMONITOR m);
			CDisplayDevice *							GetDisplayDevice(CGdiRect & nr);
			CArray<CDisplayDevice *>					GetDisplayDevices(CDisplayAdapter * a);
			
			void										EnumerateDevices();
			CDisplayMonitor *							GetMonitor(HMONITOR h);
	};
}