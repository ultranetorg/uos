#pragma once
#include "Display.h"
#include "ScreenRenderTarget.h"

namespace uos
{
	class CDirectSystem;
	
	class UOS_ENGINE_LINKING CDirectDevice : public CEngineEntity
	{
		public:
			CDirectSystem *								Engine;
			CDisplayDevice *							Display;
			ID3D11Device *								DxDevice = nullptr;
			ID3D11DeviceContext *						DxContext = nullptr;
			CArray<CScreenRenderTarget *>				Targets;
			D3D_FEATURE_LEVEL							FeatureLevel;

			CMap<int, int>								MSAA;

			bool										PresentEnabled;
			bool										SwapJumpEnabled;
			bool										VSync;

			bool										SaveFrontBuffer;
	
			UOS_RTTI
			///CDirectDevice(CEngineLevel * l, CDirectEngine * ge, CDisplayDevice * dd);
			CDirectDevice(CEngineLevel * l, CDirectSystem * ge, CDisplayDevice * dd);
			~CDirectDevice();

			CDisplayDevice *							GetDisplayDevice();
			CScreenRenderTarget *						AddTarget(CWindowScreen * w);
			void										RemoveTarget(CScreenRenderTarget * t);
			void										SwitchToGDI();
			void										TakeScreenShot(CRenderTarget * t, CString & p);

			void										SaveDepthStencil(const CString & path);

			int											GetVideoMemoryFreeAmount();
		
			void										InitCommon(CScreenRenderTarget * t);
			bool										TestVideoMemorySpeed();

			void										OnLevel1ExitRequested();
			void										OnLevel1Suspending();

			int											GetPixelSize(DXGI_FORMAT f);
			ID3D11Texture2D *							CaptureTexture(ID3D11Resource* pSource);
			CArray<char>								SaveImage(ID3D11Resource * t);

	};
}
