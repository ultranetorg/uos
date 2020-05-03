#pragma once
#include "Pipeline.h"
#include "Shader.h"
#include "DirectSystem.h"

namespace uos
{
	class CDirectPipelineFactory : public CEngineEntity
	{
		public:
			CList<CDirectPipeline *>					Pipelines;
			
			CDirectSystem *								GraphicEngine;

			CShader *									DiffuseTextureShader;
			CShader *									DiffuseColorShader;
			CShader *									TextShader;

			CDiagnostic *								Diagnostic;
			CDiagGrid									DiagGrid;

			UOS_RTTI
			CDirectPipelineFactory(CEngineLevel * l, CDirectSystem * ge);
			~CDirectPipelineFactory();
			
			void										OnDiagnosticUpdating(CDiagnosticUpdate &);
			CDirectPipeline *							GetPipeline(CShader * s);
	};
}
