#pragma once
#include "Group.h"
#include "Icon.h"

namespace uos
{
	class CGroupIcon : public CIcon<CGroup>
	{
		public:
			CWorld *									Level;
						
			UOS_RTTI
			CGroupIcon(CWorld * l, CString const & name = CGuid::Generate64(GetClassName()));
			~CGroupIcon();
	};
}
