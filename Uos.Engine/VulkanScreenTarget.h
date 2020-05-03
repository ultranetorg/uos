#pragma once
#include "RenderTarget.h"

#if 0
namespace uos
{
	class CVulkanDevice;

	struct QueueFamilyIndices
	{
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool isComplete()
		{
			return graphicsFamily >= 0 && presentFamily >= 0;
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class CVulkanScreenTarget : public CTarget
	{
		public:
			CVulkanDevice *								Device;
			CScreen *									Screen;

			VkSwapchainKHR								SwapChain;
			std::vector<VkImage>						SwapChainImages;
			VkFormat									SwapChainImageFormat;
			VkExtent2D									SwapChainExtent;
			std::vector<VkImageView>					SwapChainImageViews;
			std::vector<VkCommandBuffer>				commandBuffers;

			UOS_RTTI
			CVulkanScreenTarget(CEngineLevel * l, CVulkanDevice * d, CScreen * s);
			~CVulkanScreenTarget();

			void										Apply() override;
			void										Present() override;
			void										TakeScreenshot(CUol & o) override;

			VkSurfaceFormatKHR							chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR							chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
			VkExtent2D									chooseSwapExtent(CScreen * s, const VkSurfaceCapabilitiesKHR& capabilities);

			void										createImageViews();

			void createCommandBuffers();
	};
}
#endif
