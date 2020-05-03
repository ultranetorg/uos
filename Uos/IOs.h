#pragma once
#include "INativeMessageHandler.h"
#include "Version.h"

namespace uos
{
	class IOs
	{
		public:
			virtual CVersion							GetVersion()=0;
			virtual bool								Is64()=0;

			virtual	void								SaveDCToFile(LPCTSTR FileName, HDC src, int Width, int Height)=0;
			
			virtual										~IOs(){}
	};
}
