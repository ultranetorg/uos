#pragma once
#include "Converter.h"
#include "IType.h"

namespace uos
{
	class CSecurity : public IType
	{
		public:
			UOS_RTTI
			CSecurity();
			~CSecurity();
		
			bool										IsUserAdmin();
				
		private:
	};
}

