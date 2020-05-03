#pragma once
#include "ActiveGraph.h"
#include "ScreenEngine.h"
#include "InputSystem.h"
#include "ActiveLayer.h"

namespace uos
{
	class UOS_ENGINE_LINKING CInteractor : public CEngineEntity
	{
		public:
			CList<CInputMessage>						Deferred;
			CEvent<CInputMessage &>						MessageProcessed;

			CInputSystem *								InputEngine;
			CScreenEngine *								ViewEngine;
			CDiagnostic *								Diagnostics;
			CList<CActiveLayer *>						Layers;
			CPerformanceCounter							Pc;
			
			CActiveGraph *								FocusGraph = null;

			UOS_RTTI
			CInteractor(CEngineLevel * e, CScreenEngine * ve, CInputSystem * ie);
			~CInteractor();

			CActiveGraph *								CreateActiveGraph(const CString & name);

			CActiveLayer *								AddLayer(CScreenViewport * w, CActiveSpace * space);
			void										RemoveTarget(CActiveLayer * t);

			void										Pick(CScreen * s, CFloat2 & p, CPick * pk);
			void										Pick(CScreen * sc, CFloat2 & sp, CActiveSpace * s, CPick * cis, CPick * nis);

			void										OnDiagnosticsUpdate(CDiagnosticUpdate & a);

			void										Process(CInputMessage & m);
	};
}
