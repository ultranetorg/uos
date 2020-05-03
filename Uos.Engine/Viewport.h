#pragma once
#include "EngineLevel.h"

namespace uos
{
	class UOS_ENGINE_LINKING CViewport : public CEngineEntity
	{
		friend class CScreenEngine;

		public:
			float										TX;
			float										TY;
			float										TW;
			float										TH;

			float										W;
			float										H;

			float										MinZ = 0.f;
			float										MaxZ = 1.f;

			CList<CString>								Tags;
						
			UOS_RTTI
			CViewport(CEngineLevel * l);
			~CViewport();

			CFloat2										TargetToViewport(CFloat2 & tp);
			CFloat2										ScreenToTarget(CFloat2 & scp);
			CFloat2										ScreenToViewport(CFloat2 & sp);
			bool										Contains(CFloat2 & scp);
	};
}
