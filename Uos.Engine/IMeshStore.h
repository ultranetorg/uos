#pragma once
#include "Mesh.h"

namespace uos
{
	class IMeshStore
	{
		public:
			virtual CString								Add(CMesh * t)=0;
			virtual CMesh *								Get(const CString & name)=0;

			virtual ~IMeshStore(){}
	};
}


