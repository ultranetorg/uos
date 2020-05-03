#pragma once

namespace uos
{
	class UOS_WORLD_LINKING CWorldView : public IType, public CView
	{
		public:
			CEngine *									Engine;
			CString										Name;
			//CSize										Area;

			CString &									GetName();
			CCamera *									GetCamera(CViewport * s);

			CCamera *									AddCamera(CViewport * vp, float fov, float znear, float zfar);
			CCamera *									AddCamera(CViewport * vp, float znear, float zfar);
			
			UOS_RTTI
			CWorldView(CLevel2 * l2, CEngine * en, const CString & name);
			~CWorldView();
	};
}