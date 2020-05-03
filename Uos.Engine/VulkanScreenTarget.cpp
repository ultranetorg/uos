#include "stdafx.h"
#include "VulkanScreenTarget.h"
#include "VulkanDevice.h"

#ifdef UOS_ENGINE_VULKAN

using namespace uos;

CVulkanScreenTarget::CVulkanScreenTarget(CEngineLevel * l, CVulkanDevice * gd, CScreen * s) : CScreenRenderTarget(l)
{
	Device = gd;

	SwapChainSupportDetails swapChainSupport = Device->querySwapChainSupport();

	VkSurfaceFormatKHR	surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR	presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D			extent = chooseSwapExtent(s, swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = Device->Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = Device->findQueueFamilies();
	uint32_t queueFamilyIndices[] = {(uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily};

	if(indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if(vkCreateSwapchainKHR(Device->Device, &createInfo, nullptr, &SwapChain) != VK_SUCCESS)
	{
		throw CException(HERE, L"Failed to create swap chain");
	}

	vkGetSwapchainImagesKHR(Device->Device, SwapChain, &imageCount, nullptr);
	SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(Device->Device, SwapChain, &imageCount, SwapChainImages.data());

	SwapChainImageFormat = surfaceFormat.format;
	SwapChainExtent = extent;

	createImageViews();
}

CVulkanScreenTarget::~CVulkanScreenTarget()
{

}

void CVulkanScreenTarget::Apply()
{
	///Verify(Driver->D3DDevice->SetRenderTarget(0, BackBufferA));
}

void CVulkanScreenTarget::Present()
{
	///Verify(SwapChain->Present(null, null, null, null, 0));
}

void CVulkanScreenTarget::TakeScreenshot(CUol & o)
{

}

VkSurfaceFormatKHR CVulkanScreenTarget::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if(availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
	}

	for(const auto& availableFormat : availableFormats)
	{
		if(availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR CVulkanScreenTarget::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for(const auto& availablePresentMode : availablePresentModes)
	{
		if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
		else if(availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

#undef max
#undef min

VkExtent2D CVulkanScreenTarget::chooseSwapExtent(CScreen * s, const VkSurfaceCapabilitiesKHR& capabilities)
{
	if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = {uint32_t(s->Rect.Width), uint32_t(s->Rect.Height)};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void CVulkanScreenTarget::createImageViews()
{
	SwapChainImageViews.resize(SwapChainImages.size());

	for(size_t i = 0; i < SwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if(vkCreateImageView(Device->Device, &createInfo, nullptr, &SwapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}


void CVulkanScreenTarget::createCommandBuffers()
{
	commandBuffers.resize(SwapChainImageViews.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = Device->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if(vkAllocateCommandBuffers(Device->Device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	///for(size_t i = 0; i < commandBuffers.size(); i++)
	///{
	///	VkCommandBufferBeginInfo beginInfo = {};
	///	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	///	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	///
	///	if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
	///	{
	///		throw std::runtime_error("failed to begin recording command buffer!");
	///	}
	///
	///	VkRenderPassBeginInfo renderPassInfo = {};
	///	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	///	renderPassInfo.renderPass = renderPass;
	///	renderPassInfo.framebuffer = swapChainFramebuffers[i];
	///	renderPassInfo.renderArea.offset = {0, 0};
	///	renderPassInfo.renderArea.extent = swapChainExtent;
	///
	///	VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
	///	renderPassInfo.clearValueCount = 1;
	///	renderPassInfo.pClearValues = &clearColor;
	///
	///	vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	///
	///	vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	///
	///	vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
	///
	///	vkCmdEndRenderPass(commandBuffers[i]);
	///
	///	if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
	///	{
	///		throw std::runtime_error("failed to record command buffer!");
	///	}
	///}
}
#endif