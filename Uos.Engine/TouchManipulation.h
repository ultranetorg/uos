#pragma once
#include "EngineLevel.h"
#include "InputDevice.h"
#include "Screen.h"

namespace uos
{
	class CInputSystem;

	class CTouchManipulation : _IManipulationEvents
	{
		public:
			double m_fX;
			double m_fY;

			int m_cRefCount;
			int m_cStartedEventCount;
			int m_cDeltaEventCount;
			int m_cCompletedEventCount;

			IManipulationProcessor *		ManipulationProcessor;
			CInputDevice *					Device; 
			CInputSystem *					InputEngine;
			IConnectionPointContainer *		m_pConPointContainer;
			IConnectionPoint *				m_pConnPoint;
			CScreen *						Screen;

			CTouchManipulation(CInputSystem * ie, CInputDevice * id, IManipulationProcessor * manip, CScreen * sc);
			~CTouchManipulation();

			int											GetStartedEventCount();
			int											GetDeltaEventCount();
			int											GetCompletedEventCount();
			double										GetX();
			double										GetY();

			virtual HRESULT STDMETHODCALLTYPE			ManipulationStarted(FLOAT x, FLOAT y);

			virtual HRESULT STDMETHODCALLTYPE			ManipulationDelta(	FLOAT x,
																			FLOAT y,
																			FLOAT translationDeltaX,
																			FLOAT translationDeltaY,
																			FLOAT scaleDelta,
																			FLOAT expansionDelta,
																			FLOAT rotationDelta,
																			FLOAT cumulativeTranslationX,
																			FLOAT cumulativeTranslationY,
																			FLOAT cumulativeScale,
																			FLOAT cumulativeExpansion,
																			FLOAT cumulativeRotation);

			virtual HRESULT STDMETHODCALLTYPE			ManipulationCompleted(	FLOAT x,
																				FLOAT y,
																				FLOAT cumulativeTranslationX,
																				FLOAT cumulativeTranslationY,
																				FLOAT cumulativeScale,
																				FLOAT cumulativeExpansion,
																				FLOAT cumulativeRotation);

			STDMETHOD_(ULONG, AddRef)(void);
			STDMETHOD_(ULONG, Release)(void);
			STDMETHOD(QueryInterface)(REFIID riid, LPVOID * ppvObj);
	};
}
