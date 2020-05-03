#pragma once

namespace uos
{
	enum class EPipelineStage : int
	{
		Vertex = 0b0001, Pixel = 0b0010
	};

	struct CShaderDeclaration
	{
		CString						Name;
		CString						Type;
		int							Size;
		CString						Semantics;
		CAnsiString					AnsiSemantics;
		int							SemanticsIndex;
		CString						Format;
		EPipelineStage				Stages;

		void Save(CXon * p)
		{
			p->Set(Type);
			p->Add(L"Semantics")->Set(Semantics);
			p->Add(L"SemanticsIndex")->Set(SemanticsIndex);
			p->Add(L"Format")->Set(Format);
			p->Add(L"Stages")->Set(int(Stages));
		}

		void Load(CXon * p)
		{
			Name			= p->Name;
			Type			= p->AsString();
			Semantics		= p->Get<CString>(L"Semantics");
			SemanticsIndex	= p->Get<CInt32>(L"SemanticsIndex");
			Format			= p->Get<CString>(L"Format");
			Stages			= (EPipelineStage)(int)p->Get<CInt32>(L"Stages");
		}
	};

	struct CBufferDeclaration
	{
		CString						Name;
		CArray<CShaderDeclaration>	Constants;
	};

	class UOS_ENGINE_LINKING CShader : public virtual CShared, public virtual IType
	{
		public:
			CString										Name;
			CArray<CBufferDeclaration>					Buffers;

			CArray<CShaderDeclaration>					Textures;
			CArray<CShaderDeclaration>					Samplers;

			CArray<CShaderDeclaration>					VertexInput;
			CArray<CShaderDeclaration>					VertexOutput;
			CString										VertexProgram;
			CString										PixelProgram;

			bool										IsChanged = false;
			
			bool										operator == (const CShader & t){ return VertexProgram == t.VertexProgram && PixelProgram == t.PixelProgram; }

			UOS_RTTI
			CShader(CString const & name = CGuid::Generate64(GetClassName()));
			~CShader();

			int											SizeByType(CString const & name, CString const & f);
			bool										UsesStage(int buffer, EPipelineStage stages);
			int											AddConstantBuffer(const wchar_t * name);
			void										AddConstant(int buffer, CString const & name, CString const & type, EPipelineStage stages);
			void										AddVertexInput(CString const & name, CString const & type, CString const & sem, CString const & f = L"");
			void										AddVertexOutput(CString const & name, CString const & type, CString const & sem);

			void										AddTexture(CString const & name, CString const & type);
			void										AddSampler(CString const & name);
			
			CString										AssembleVertexProgram();
			CString										AssemblePixelProgram();

			CArray<D3D11_INPUT_ELEMENT_DESC>			AssebleInputLayout();

			int											GetConstantBufferSize(CString const & cbuffer);
			int											FindConstantBuffer(CString const & cbuffer);
			//int											FindConstant(int cbuffer, CString const & name);
			int											FindTexture(CString const & name);
			int											FindSampler(CString const & name);
		
			void Save(CXon * r);
			void Load(CXon * r);
	};
}