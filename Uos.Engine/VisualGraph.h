#pragma once
#include "VisualSpace.h"
#include "RenderTarget.h"
#include "Visual.h"

namespace uos
{
	class CVisualGraph : public CEngineEntity
	{
		public:
			CString										Name;

			virtual void								AddNode(CVisualSpace * s, CVisual * v)=0;
			virtual void								RemoveNode(CVisualSpace * s, CVisual * v)=0;
		
			virtual void								AddSpace(CVisualSpace * s)=0;
			virtual void								RemoveSpace(CVisualSpace * s)=0;
			
			virtual void								Render(CRenderTarget * t, CViewport * vp, CVisualSpace * s)=0;

			UOS_RTTI
			CVisualGraph(CEngineLevel * l, const CString & name);
			~CVisualGraph();
	};
}