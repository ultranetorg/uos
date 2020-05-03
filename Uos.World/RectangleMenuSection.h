#pragma once
#include "RectangleMenuItem.h"

namespace uos
{
	class CRectangleSectionMenuItem;

	class UOS_WORLD_LINKING CRectangleMenuSection : public CRectangle, public IMenuSection
	{
		public:
			CRefList<CRectangleMenuItem *>				Items;
			CRectangleMenuItem *						Highlighted = null;
			CVisual	*									Highlighter = null;
			CStyle *									Style;
			CArea *										Area = null;
			
			UOS_RTTI
			CRectangleMenuSection(CWorldLevel * w, CStyle * s, const CString & name = GetClassName());
			virtual ~CRectangleMenuSection();

			virtual void								Open(CArea * a, CElement * p, CPick * pick, float l, float r, float u, float d) override;
			void										Open(CElement * pnt);
			virtual void								Close() override;
			virtual bool								IsOpen() override;
			virtual void								Clear() override;

			virtual IMenuItem *							AddItem(const CString & text) override;
			virtual void								AddItem(IMenuItem * item) override;
			virtual IMenuItem *							AddSeparator() override;
			CRectangleTextMenuItem *					AddStandardItem(CTexture * icon, CString const & text);
			ISectionMenuItem *							AddSectionItem(CString const & text) override;

			void										RemoveItem(IMenuItem * item);

			void										OnItemCursorMoved(CActive * r, CActive * s, CMouseArgs * a);
			void										Unhighlight();
			void										OnStateModified(CActive *, CActive *, CActiveStateArgs *);
			void										OnMouse(CActive * r, CActive * s, CMouseArgs * a);
	};

}
