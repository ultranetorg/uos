#pragma once
#include "CanvasElement.h"
#include "Style.h"

namespace uos
{
	class UOS_WORLD_LINKING CText : public CCanvasElement
	{
		public:
			CText(CWorldLevel * l, CStyle * s, CString const & name = GetClassName(), bool active = false);
			~CText();
	};
}

