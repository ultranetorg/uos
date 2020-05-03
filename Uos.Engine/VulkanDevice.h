#pragma once
#include "Display.h"
#include "WindowScreen.h"
#include "VulkanScreenTarget.h"

namespace uos
{
#if 0
	class CVulkanEngine;

	class CVulkanDevice : public CEngineEntity
	{
		public:
			CVulkanEngine *								Engine;
			CDisplayDevice *							DisplayDevice;
			//CGraphicDriverCache						Cache;
			CArray<CScreenTarget *>						Targets;

			bool										VSync;
			
			VkPhysicalDevice							PhysicalDevice;
			VkDevice									Device;
			VkSurfaceKHR								Surface;
			VkCommandPool								commandPool;

			const CArray<const char*>					Extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

			bool										SaveFrontBuffer = false;
			

			VkQueue										graphicsQueue;
			VkQueue										presentQueue;


			UOS_RTTI
			CVulkanDevice(CEngineLevel * l, CVulkanEngine * ge, CDisplayAdapter * a, VkSurfaceKHR surface);
			~CVulkanDevice();

			QueueFamilyIndices							findQueueFamilies();
			bool										isDeviceSuitable();
			bool										checkDeviceExtensionSupport();
			SwapChainSupportDetails						querySwapChainSupport();
			void										createCommandPool();

			CScreenTarget *								GetPrimaryTarget();
			CScreenTarget *								AddTarget(CWindowScreen * w);
			void										RemoveTarget(CScreenTarget * t);
			void										SwitchToGDI();
			void										Present();
			void										TakeScreenShot(CScreenTarget * t, CUol & p);

			void										OnLevel1ExitQueried();
			void										OnLevel1SuspendingStarted();
			void										OnLevelCreated(int n, ILevel * l);

	};
#endif
}
