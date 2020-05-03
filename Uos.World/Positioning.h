#pragma once
#include "WorldLevel.h"
//#include "Allocation.h"
//#include "Model.h"

namespace uos
{
	struct CPositioningCapture
	{
		CSize										Size;
		CFloat3										Point;
		CFloat3										Offset;
		float										Angle;
		CActiveSpace *								Space;
	};

	class CPositioning : public virtual IType, public virtual CShared
	{
		public:
			std::function<CView *()>					GetView;

			virtual	CFloat3								GetPoint(CViewport * vp, CFloat2 & vpp)=0;
			virtual	CMatrix								GetMatrix(CViewport * vp)=0;
			virtual	CFloat3								Project(CViewport * vp, CActiveSpace * s, CFloat3 & p)=0;
			virtual CPositioningCapture					Capture(CPick & pick, CSize & size, CMatrix & wm)=0;
			virtual CPositioningCapture					Capture(CPick & pick, CSize & size, CFloat3 & offset)=0;
			virtual CTransformation						Move(CPositioningCapture & c, CPick & pick)=0; // output is a new position of a node in this CS

			UOS_RTTI
			virtual ~CPositioning(){}
	};
}