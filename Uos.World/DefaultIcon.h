#pragma once
#include "Icon.h"

namespace uos
{
	class UOS_WORLD_LINKING CDefaultIcon : public CIcon<CWorldEntity>
	{
		public:
			UOS_RTTI
			CDefaultIcon(CWorld * l, CString const & name = CGuid::Generate64(GetClassName()));
			virtual ~CDefaultIcon();

			void										SetEntity(CUol & e) override;

	};
}