#pragma once
#include "Texture.h"
#include "Shader.h"

namespace uos
{
	class CMaterial;

	class IMaterialStore
	{
		public:
			virtual CString								Add(CMaterial * t)=0;
			virtual void								Save(CXon *)=0;
			virtual void								Load(CXon * p)=0;
			virtual CTexture *							GetTexture(const CString & name)=0;
			virtual CMaterial *							GetMaterial(const CString & name)=0;
			virtual CShader *							GetShader(const CString & name)=0;

			virtual ~IMaterialStore(){}
	};
}


