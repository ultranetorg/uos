#pragma once
#include "DisplaySystem.h"
#include "DirectDevice.h"

namespace uos
{
	class UOS_ENGINE_LINKING CDirectSystem : public CDisplaySystem
	{
		public:
			CArray<CDirectDevice *>						Devices;
			IPerformanceCounter *						PcUpdate;
			CXon *										Parameter;
			CDisplayDevice *							DefaultDevice;
				
			UOS_RTTI
			CDirectSystem(CEngineLevel * e);
			virtual ~CDirectSystem();

			CDirectDevice *								GetAppropriateDevice(CWindowScreen * w);
			CDirectDevice *								GetAppropriateDevice(CDisplayDevice * dd);
			//void										RemoveTarget(CScreenTarget * t) override;
	};
}