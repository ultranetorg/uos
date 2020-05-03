#pragma once
#include "TreeVisualGraph.h"
#include "PipelineFactory.h"

namespace uos
{
	class UOS_ENGINE_LINKING CDirectTreeVisualGraph : public CTreeVisualGraph
	{
		public:
			CMaterial *									StencilMaterial;
			CDirectPipeline *							StencilPipeline;	

			CMap<CDirectPipeline *, int>				Pipelines;
			CDirectPipeline *							ClippingPipeline;
			CDirectConstantBuffer	*					ClippingBuffer;
			int											ClippingWVPSlot;

			CMap<CDirectDevice *, ID3D11DepthStencilState *>	DxNoClipping;
			CMap<CDirectDevice *, ID3D11DepthStencilState *>	DxApplyClipping;
			CMap<CDirectDevice *, ID3D11DepthStencilState *>	DxRootIndex;
			CMap<CDirectDevice *, ID3D11DepthStencilState *>	DxIncrementIndex;

			CMap<CDirectDevice *, ID3D11RasterizerState *>		DxRasterizerState;
			CMap<CDirectDevice *, ID3D11BlendState *>			DxBlendStateNormal;
			CMap<CDirectDevice *, ID3D11BlendState *>			DxBlendStateAlpha;
			CMap<CDirectDevice *, ID3D11BlendState *>			DxClipperBlending;



			UOS_RTTI
			CDirectTreeVisualGraph(CEngineLevel * l, CDirectPipelineFactory * pf, const CString & name);
			~CDirectTreeVisualGraph();

			void										AssignPipelines(CVisualSpace * s, CRenderTarget * t, CVisual * root, CVisual * i);

			void										Render(CRenderTarget * t, CViewport * vp, CVisualSpace * s) override;

			void										Render(CVisualSpace * s, CRenderTarget * t, CViewport * vp, CCamera * c, CDirectPipeline * p, CVisual * root, CVisual * v, int & index, bool alpha);
			void										RenderVisual(CDirectDevice * d, CVisualSpace * s, CVisual * v, CCamera * c, int & index);
			void										RenderClipping(CDirectDevice * d, CVisualSpace * s, CVisual * v, CCamera * c, int & index);
	};
}