#pragma once
#include "AvatarCard.h"
#include "Sizer.h"
#include "World.h"
#include "Fieldable.h"
#include "RectangleMenu.h"

namespace uos
{
	class UOS_WORLD_LINKING CWidgetWindow : public CModel, public CFieldable
	{
		public:
			CWorld *									World;
			CObject<CWorldEntity>						Entity;
			CStyle *									Style;
			CElement *									Face = null;
			CRectangleSizer 							Sizer;
			CRectangleMenu *							Menu = null;

			UOS_RTTI
			CWidgetWindow(CWorld * l, CServer * sys, CStyle * s, const CString & name = CGuid::Generate64(GetClassName()));
			~CWidgetWindow();

			void										SetFace(CElement * e);
			virtual void								SetEntity(CUol & o) override;
			void										OnDependencyDestroying(CNexusObject *);

			virtual void								SaveInstance() override;
			virtual void								LoadInstance() override;

			virtual void								Place(IFieldOperations * s) override;
			
			virtual void								OnMouse(CActive *, CActive *, CMouseArgs *);

			void DetermineSize(CSize & smax, CSize & s) override;

	};
}
