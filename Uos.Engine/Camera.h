#pragma once
#include "EngineLevel.h"
#include "Viewport.h"

namespace uos
{
	enum class EProjection
	{
		Ortho = 1, Perspective = 2
	};

	static wchar_t * ToString(EProjection e)
	{
		static wchar_t * name[] = {L"Undefined", L"Ortho", L"Perspective"};
		return name[int(e)];
	}

	class UOS_ENGINE_LINKING CCamera : public CEngineEntity
	{
		public:
			CString										Name;
			CViewport *									Viewport;
			EProjection									Projection;

			float										Fov;
			float										ZNear;
			float										ZFar;
			CFloat3										Position = {0, 0, 0};
			CFloat3										Rotation = {0, 0, 0};
			CFloat3										Direction = {0, 0, 1};;
			CFloat3										Up = {0, 1, 0};;

			std::function<CSize()>						ViewportFormula;

			std::function<CMatrix()>					ViewFormula;
			CMatrix										ViewMatrix;
			CMatrix										ViewMatrixInvers;

			CEvent<CCamera *>							Moved;
			CEvent<CCamera *>							Modified;
			
			CMatrix										ProjectionMatrix;
			CMatrix										ViewProjectionMatrix;
			float										Aspect;
			CFloat2										Offset = {0, 0};

			CFloat4										Frustum[6];

			UOS_RTTI
			CCamera(CEngineLevel * l, CViewport * vp, CXon * p);
			CCamera(CEngineLevel * l, CViewport * vp, const CString & name, float fov, float znear, float zfar);
			CCamera(CEngineLevel * l, CViewport * vp, const CString & name, float znear, float zfar);
			virtual ~CCamera();

			void										UseAffine();
			void										UseLookToLeftHand();

			void										SetFov(float fov);
			void										SetUp(CFloat3 & up);
			void										SetDirection(CFloat3 & dir);
			void										SetPosition(CFloat3 & pos);
			void										SetRotation(CFloat3 & r);


			void										Rotate(CQuaternion & dr);

			void										OnDiagnosticsUpdateStarted(CDiagnostic * d);
			
			float										GetHFov();
			float										GetVFov();
			float										GetAspect();
			void										SetOffset(CFloat2 & o);

			CFloat3										ProjectVertex(CFloat3 & in);
			CFloat2										ProjectVertexXY(CFloat3 & in); // project to the viewport, not to the screen
			CFloat3										UnprojectVertex(CFloat3 & in);
			
			CRay										Raycast(CFloat2 & vpp);
			CRay										Raycast();
			CRay										GetScanRayPCenterDelta(CFloat2 & vpp);//!!! p.x = [-w/2..+w/2], p.y = [-h/2..+h/2], 
			CMatrix	&									GetViewProjectionMatrix();

			float										UnprojectZ(float x, float sx);
			float										UnprojectX(float z, float sx);
			float										UnprojectY(float z, float sy);

			void										UpdateViewProjection();

			bool										Contains(CFloat3 & p);
			bool										Contains(CFloat3 & a, CFloat3 & b, CFloat3 & c, bool backcull);
			bool										RectangleIntersectsTriangle(CRect & r, CFloat2 & a, CFloat2 & b, CFloat2 & c);
	};
}