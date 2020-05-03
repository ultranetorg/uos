#pragma once
#include "Shader.h"
#include "DirectDevice.h"

namespace uos
{
	class CDirectConstantBuffer
	{
		public:
			CShader *									Shader = null;
			CArray<char>								Data;								
			int											Slot = -1;
			CMap<CDirectDevice *, ID3D11Buffer *>		DxBuffers;
			CMap<CDirectDevice *, bool>					IsChanged;

			bool										VStage = false;
			bool										PStage = false;

			CDirectConstantBuffer();
			~CDirectConstantBuffer();

			void										Bind(CShader * sh, CString const & cbuffer);
			void										Apply(CDirectDevice * d);
			
			int											FindSlot(CString const & name);

			void										SetValue(int p, float v);
			void										SetValue(int p, CFloat3 & v);
			void										SetValue(int p, CFloat4 & v);
			void										SetValue(int p, CMatrix & v);
	};
}
