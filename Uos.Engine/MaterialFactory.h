#pragma once
#include "Material.h"
#include "PipelineFactory.h"

namespace uos
{
	#define UOS_PIXEL_PROGRAM_DIFFUSE_COLOR			
	#define UOS_PIXEL_PROGRAM_DIFFUSE_TEXTURE		
	
	class UOS_ENGINE_LINKING CMaterialFactory : public CEngineEntity
	{
		public:
			CArray<CMaterial *>							Materials;
			CDirectSystem *								GraphicEngine;
			CDirectPipelineFactory *					PipelineFactory;

			UOS_RTTI
			CMaterialFactory(CEngineLevel * l, CDirectSystem * ge, CDirectPipelineFactory * sf);
			~CMaterialFactory();
	};
}
