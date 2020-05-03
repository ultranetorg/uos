#pragma once
#include "ScreenRenderTarget.h"

namespace uos
{
	class UOS_ENGINE_LINKING CScreenViewport : public CViewport
	{
		public:
			CScreenRenderTarget *						Target;

			CScreenViewport(CEngineLevel * l, CScreenRenderTarget * s, float x, float y, float w, float h, float rw, float rh) : CViewport(l)
			{
				Target = s;
				Tags = Target->Screen->Tags;

				TX = x;
				TY = y;

				W = w;
				H = h;

				TW = rw;
				TH = rh;
			}

			void Apply()
			{
				Target->SetViewport(TX, Target->Size.H - TY - TH, TW, TH, MinZ, MaxZ);
			}
	};
}
