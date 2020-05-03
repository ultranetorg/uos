#pragma once

namespace uos
{
	class CDisplayMonitor
	{
		public:
			CString			Name;
			HMONITOR		Handle;
			bool			IsPrimary;
			
			CDisplayMonitor(HMONITOR h)
			{
				Info.cbSize = sizeof(MONITORINFOEX);
				GetMonitorInfo(h, &Info);

				Handle		= h;
				Name		= GetDisplayMonitorName(Info.szDevice);
				IsPrimary	= (Info.dwFlags & MONITORINFOF_PRIMARY) != 0;
			}
			
			CString GetDisplayMonitorName(const CString & ddName)
			{
				DISPLAY_DEVICE dd;
				dd.cb = sizeof(DISPLAY_DEVICE);

				for(int i=0; i<24; i++)
				{
					if(EnumDisplayDevices(ddName.c_str(), i, &dd, 0))
					{
						if(dd.StateFlags & (DISPLAY_DEVICE_ACTIVE | DISPLAY_DEVICE_ATTACHED))
						{
							return dd.DeviceString;
						}
					}
					else
					{
						break;
					}
				}
				return L"<Monitor Not Found>";
			}
			
			RECT GetNRect()
			{
				return Info.rcMonitor;
			}

			RECT GetNRectWork()
			{
				return Info.rcWork;
			}
			
		private:
			MONITORINFOEX	Info;
	};
	
	class CDisplayAdapter
	{
		public:
			IDXGIAdapter1 *	DxAdapter;
			LUID			Luid;
			CString			Name;
			__int64			MemorySize;
	};

	class CDisplayDevice
	{
		public:
			CString										Name;
			int											RefreshRate;
			DXGI_FORMAT									Format;
			CGdiRect									NRect;
			CGdiRect									NRectWork;
			CSize										Resolution;
			CDisplayAdapter	*							Adapter;
			CDisplayMonitor *							Monitor;
			CFloat2										Dpi;
			CFloat2										Scaling;
	};

}