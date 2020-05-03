#pragma once
#include "IWriterProgress.h"
#include "String.h"

namespace uos
{
	class IWriter
	{
		public:
			virtual void								Write(void * src, int size) = 0;
			virtual void								Write(void * src, int size, IWriterProgress * p) = 0;
			virtual void								Write(const CString & src, IWriterProgress * p) = 0;
			
	};
}
