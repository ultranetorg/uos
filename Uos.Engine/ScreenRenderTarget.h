#pragma once
#include "RenderTarget.h"
#include "WindowScreen.h"
//#include "Viewport.h"

namespace uos
{
	class CDirectDevice;

	class UOS_ENGINE_LINKING CScreenRenderTarget : public CRenderTarget
	{
		public:
			CScreen *									Screen;

			CEngineLevel *								Level;
			CDirectDevice *								Device;
			IDXGISwapChain *							SwapChain = null;
			ID3D11Texture2D *							BackBufferA = null;
			ID3D11RenderTargetView *					RenderTargetView = null;
			ID3D11DepthStencilView *					DepthStencilView = null;
			ID3D11Texture2D *							DepthStencil = null;
			CSize										Size;

			UOS_RTTI
			CScreenRenderTarget(CEngineLevel * l, CDirectDevice * gd, CWindowScreen * s);
			~CScreenRenderTarget();

			void										Apply();
			void										Present();
			void										TakeScreenshot(CString & o);
			void										SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth);
	};
}
