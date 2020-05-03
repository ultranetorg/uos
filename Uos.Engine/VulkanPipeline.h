#pragma once

#ifdef UOS_ENGINE_VULKAN

#include "Viewport.h"
#include "VulkanDevice.h"

namespace uos
{
	class CVulkanPipeline : public virtual IType
	{
		public:
			CEngineLevel *								Level;
			VkPipelineLayout							pipelineLayout;
			VkPipeline									graphicsPipeline;
			VkRenderPass								renderPass;
			CVulkanScreenTarget	*						Target;						
			std::vector<VkFramebuffer>					swapChainFramebuffers;

			UOS_RTTI
			CVulkanPipeline(CEngineLevel * l, CVulkanScreenTarget * t, CViewport & vp, CArray<char> & vert, CArray<char> & frag);
			~CVulkanPipeline();

			VkShaderModule								CreateShaderModule(const std::vector<char>& code);
			void createFramebuffers();
			void createRenderPass();

	};
}
#endif // UOS_ENGINE_VULKAN