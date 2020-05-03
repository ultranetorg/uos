#pragma once
#include "VisualSpace.h"
#include "ScreenViewport.h"

namespace uos
{
	class CScreenRenderLayer : public CEngineEntity
	{
		public:
			//CRenderTarget *								Target;
			CVisualSpace *								Space;
			CScreenViewport *							Viewport;
			
			UOS_RTTI
			CScreenRenderLayer(CEngineLevel * l) : CEngineEntity(l){}
			~CScreenRenderLayer(){}
	};
}
