#pragma once
#include "ScreenEngine.h"
#include "PipelineFactory.h"
#include "RenderLayer.h"
#include "ScreenViewport.h"

namespace uos
{
	class UOS_ENGINE_LINKING CRenderer : public CEngineEntity
	{
		public:
			CDirectPipelineFactory *					PipelineFactory;
			IPerformanceCounter *						PcUpdate;
			CDiagnostic *								Diagnostic;
			CDirectSystem *								GraphicEngine;
			CList<CScreenRenderLayer *>						Layers;
			CList<CScreenRenderTarget *>				ScreenTargets;

			UOS_RTTI
			CRenderer(CEngineLevel * l, CDirectSystem * ge, CDirectPipelineFactory * sf, CMaterialFactory * mf);
			~CRenderer();

			CScreenRenderLayer *								AddLayer(CScreenViewport * w, CVisualSpace * space);
			CScreenRenderLayer *								AddLayer(CDisplayDevice * dd);
			void										RemoveTarget(CScreenRenderLayer * t);
		
			void										Update();
			void										RenderSpace(CRenderTarget * t, CViewport * vp, CVisualSpace * s);

			void										OnDiagnosticsUpdate(CDiagnosticUpdate & a);
	};
}