#pragma once

namespace uos
{
	class IUwmProtocol
	{
		public:
			virtual CElement *						CreateElement(CString const & name, CString const & type)=0;

			virtual ~IUwmProtocol(){}
	};
}