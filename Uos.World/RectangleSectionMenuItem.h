#pragma once
#include "RectangleMenuSection.h"

namespace uos
{
	class UOS_WORLD_LINKING CRectangleSectionMenuItem : public CRectangleTextMenuItem, public ISectionMenuItem
	{
		public:
			CRectangleMenuSection *						Section = null;
			
			UOS_RTTI
			CRectangleSectionMenuItem(CWorldLevel * l, CStyle * s, CString const & title, const CString & name = GetClassName());
			~CRectangleSectionMenuItem();

			IMenuSection *								GetSection() override { return Section; }

			static CMesh *								CreateArrowMesh(CWorldLevel * w);

			void										HighlightArrow(bool e, CSize & area);
			virtual void								Highlight(CArea * a, bool e, CSize & s, CPick * p) override;
			virtual void								SetSection(IMenuSection * m);
			//virtual CSize								UpdateContentLayout(CLimits const & l, bool apply) override;
	};

}
