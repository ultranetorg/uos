#pragma once
#include "DirectSystem.h"
#include "VulkanDevice.h"

namespace uos
{
#if 0
	class CVulkanEngine : public CDirectEngine
	{
		public:
			VkInstance									Instance;
			VkDebugUtilsMessengerEXT callback;
			CArray<VkPhysicalDevice>					PhysicalDevices;
			CList<VkSurfaceKHR>							Surfaces;

			const CArray<const char*>					validationLayers =	{ "VK_LAYER_LUNARG_standard_validation" };
			const CArray<const char*>					glfwExtensions = {	VK_KHR_SURFACE_EXTENSION_NAME,
																			VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
																			VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
																			VK_KHR_WIN32_SURFACE_EXTENSION_NAME};

			CArray<CVulkanDevice *>						Devices;

			#ifdef NDEBUG
				const bool enableValidationLayers = false;
			#else
				const bool enableValidationLayers = true;
			#endif

			UOS_RTTI
			CVulkanEngine(CEngineLevel * e);
			~CVulkanEngine();

			std::vector<const char*>					GetRequiredExtensions();
			bool										CheckValidationLayerSupport();
			VkResult									SetupDebugCallback();
			void										DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

			CScreenTarget *								AddTarget(CDisplayDevice * dd) override;
			CScreenTarget *								AddTarget(CWindowScreen * w) override;
	};
#endif

}
