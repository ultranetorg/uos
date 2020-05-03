#pragma once
#include "EngineLevel.h"
#include "InputDevice.h"

namespace uos
{
	class CInputSystem;

	class UOS_ENGINE_LINKING CKeyboard : public CEngineEntity, public CInputDevice, public INativeMessageHandler
	{
		public:
			CInputSystem *								InputEngine;
			EInputAction								GetPressState(EControl vk);
			EInputAction								GetToggleState(EControl vk);
			bool										ProcessMessage(MSG * m);

			UOS_RTTI
			CKeyboard(CEngineLevel * ew, CInputSystem * ie);
			~CKeyboard();
	};
}
