#pragma once
#include "EngineLevel.h"
#include "Shader.h"
#include "DirectDevice.h"
#include "ConstantBuffer.h"
#include "Visual.h"

namespace uos
{

	class CDirectPipeline : public virtual CShared, public CEngineEntity
	{
		public:
			typedef HRESULT(WINAPI* PFN_D3DCOMPILE)(LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO*, ID3DInclude*, LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**);

			CShader	*									Shader;

			CMap<CDirectDevice *, ID3D11VertexShader *>	VShaders;
			CMap<CDirectDevice *, ID3D11PixelShader *>	PShaders;
			CMap<CDirectDevice *, ID3D11InputLayout *>	Layouts;

			CDirectConstantBuffer *						GlobalConstants;

			CList<CVisual *>							Visuals;
			
			CArray<D3D11_INPUT_ELEMENT_DESC> 			Layout;

			HMODULE										Compiler;
			PFN_D3DCOMPILE								D3DCompile;

			ID3DBlob *									VCode;
			ID3DBlob *									PCode;

			int											Time = -1;	

			UOS_RTTI
			CDirectPipeline(CEngineLevel * l, CShader * s);
			~CDirectPipeline();

			void										Apply(CDirectDevice * gd);

			ID3DBlob *									CompileShader(CAnsiString const& code, CAnsiString const& entry_point, CAnsiString const& model);


	};
}
