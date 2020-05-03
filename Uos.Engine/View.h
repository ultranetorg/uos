#pragma once
#include "Camera.h"

namespace uos
{
	class CView
	{
		public:
			CList<CCamera *>							Cameras;
			CCamera *									PrimaryCamera = null;

			virtual	CString &							GetName()=0;
			virtual CCamera *							GetCamera(CViewport * s)=0;

			virtual ~CView(){}
	};
}