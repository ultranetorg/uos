#pragma once
#include "InputDevice.h"
#include "TouchManipulation.h"
#include "ScreenEngine.h"

namespace uos
{
	#define TOUCHEVENTFMASK_CONTACTAREA 0x0004
	#define MOUSEEVENTF_FROMTOUCH 0xFF515700

	class CInputSystem;

	struct CTouch
	{
		int			Id = -1;
		bool		Primary = false;
		CFloat2		Position;
		CFloat2		Origin;
		CFloat2		Size;
	};

	enum class ETouchAction
	{
		Null, Added, Removed, Movement
	};

	struct UOS_ENGINE_LINKING CTouchInput : public CShared
	{
		//EMoveStage				Stage;

		ETouchAction			Action;
		CTouch *				Touch;
		CList<CTouch *> *		Touches;

		//CFloat2					Position;
		//CFloat2					TranslationDelta;
		//CFloat2					ScaleDelta;
		//CFloat2					ExpansionDelta;
		//CFloat2					RotationDelta;
		//CFloat2					CumulativeTranslation;
		//CFloat2					CumulativeScale;
		//CFloat2					CumulativeExpansion;
		//CFloat2					CumulativeRotation;
	};


	class CTouchScreen : public CEngineEntity, public CInputDevice, public INativeMessageHandler
	{
		public:
			CInputSystem *					InputEngine;
			CScreenEngine *					ScreenEngine;
			CWindowScreen *					Screen;
			IManipulationProcessor *		ManipProcessor;
			CTouchManipulation *			ManipulationEventSink;
			CArray<TOUCHINPUT>				Inputs;
			CList<CTouch *>					Touches;

			UOS_RTTI
			CTouchScreen(CEngineLevel * ew, CInputSystem *, CWindowScreen * w);
			~CTouchScreen();

			bool							ProcessMessage(MSG * m) override;
	};
}
