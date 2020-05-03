#pragma once
#include "InputDevice.h"
#include "ScreenEngine.h"

namespace uos
{
	class CInputSystem;

	class UOS_ENGINE_LINKING CMouse : public CEngineEntity, public CInputDevice, public INativeMessageHandler
	{
		public:
			CInputSystem *								InputEngine;
			CScreenEngine *								ScreenEngine;

			CFloat2										Position;		// viewport 2d position
			CScreen *									Screen = null;

			HCURSOR										Default;
			HCURSOR										Image;


			EInputAction								GetKeyState(EControl vk);
			bool										ProcessMessage(MSG * m);

			UOS_RTTI
			CMouse(CEngineLevel * ew, CScreenEngine * se, CInputSystem * ie);
			~CMouse();
			
			void ApplyCursor();
			void SetImage(HCURSOR c);
			void ResetImage();
	};
}
