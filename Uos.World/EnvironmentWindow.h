#pragma once
#include "RectangleMenu.h"
#include "Stack.h"
#include "Sizer.h"

namespace uos
{
	class UOS_WORLD_LINKING CEnvironmentWindow : public CModel
	{
		public:
			CWorld * 									Level;
			CElement *									Content = null;
			CRectangleMenu *							ServiceMenu;
			CStyle *									Style;
			CRectangleSizer	*							Sizer = null;
			CObject<CWorldEntity>						Entity;
			CServer *									Server;

			UOS_RTTI
			CEnvironmentWindow(CWorld * l, CServer * sys, CStyle * s, const CString & name = CGuid::Generate64(GetClassName()));
			~CEnvironmentWindow();

			using CElement::Load;

			//using										CWorldNode::UpdateLayout;
			virtual void								SetEntity(CUol & e) override;

			void										OnDependencyDestroying(CNexusObject * o);
			void										OnMouse(CActive *, CActive *, CMouseArgs *);
			void										OnIconCursorMoved(CActive * r, CActive * s, CMouseArgs * a);

			void										SetContent(CElement * n);

			virtual void								LoadProperties(CStyle * s, CXon * n) override;
			virtual void								LoadNested(CStyle * s, CXon * n, std::function<CElement *(CXon *, CElement *)> & load) override;

			virtual void								DetermineSize(CSize & smax, CSize & s) override;
			virtual CTransformation						DetermineTransformation(CPositioning * ps, CPick & pk, CTransformation & t) override;

			virtual void								UseSizer(CElement * tagter, CElement * gripper);

	};
}
