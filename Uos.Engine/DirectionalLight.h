#pragma once
#include "Light.h"

namespace uos
{
	class CDirectPipeline;

	class UOS_ENGINE_LINKING CDirectionalLight : public CLight
	{
		public:
			CFloat3										Direction;
			float										DiffuseIntensity;

			void										Apply(CDirectPipeline * p);
			void										BuildShaderCode(CShader & c);

			void										SetDirection(CFloat3 & d);
			void										SetDiffuseIntensity(float i);
		
			CDirectionalLight(CEngineLevel * e);
			virtual ~CDirectionalLight();
	};
}