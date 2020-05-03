#include "StdAfx.h"
#include "VulkanEngine.h"

#ifdef UOS_ENGINE_VULKAN

using namespace uos;

CVulkanEngine::CVulkanEngine(CEngineLevel * e) : CDirectSystem(e)
{
	if(enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	auto appname = CConverter::ToAnsi(e->Level0->Product->Name);

	VkApplicationInfo appInfo = {};
	appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName	= appname.data();
	appInfo.applicationVersion	= VK_MAKE_VERSION(e->Level0->ProductInfo->Version.Era, e->Level0->ProductInfo->Version.Release, e->Level0->ProductInfo->Version.Build);
	appInfo.pEngineName			= "Aximion";
	appInfo.engineVersion		= VK_MAKE_VERSION(e->Server->Info->Version.Era, e->Server->Info->Version.Release, e->Server->Info->Version.Build);
	appInfo.apiVersion			= VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if(enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if(vkCreateInstance(&createInfo, nullptr, &Instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}

	/// Enumerate Devices

	SetupDebugCallback();

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

	if(deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	PhysicalDevices.resize(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, PhysicalDevices.data());
}

CVulkanEngine::~CVulkanEngine()
{
	if(enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(Instance, callback, nullptr);
	}
	vkDestroyInstance(Instance, nullptr);
}

CScreenRenderTarget * CVulkanEngine::AddTarget(CDisplayDevice * dd)
{
	//CGraphicDeviceDirect3D9 * d = new CGraphicDeviceDirect3D9(Level, this, DdKernel, dd);
	//d->Index = Drivers.size();
	//Drivers.push_back(d);
	//return d->GetPrimaryTarget();

	return null;
}

CScreenRenderTarget * CVulkanEngine::AddTarget(CWindowScreen * w)
{
	VkSurfaceKHR				surface;
	VkWin32SurfaceCreateInfoKHR sci;
	//PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
	//
	//vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(Instance, "vkCreateWin32SurfaceKHR");
	//
	//if(!vkCreateWin32SurfaceKHR)
	//{
	//	throw CException(HERE, L"Vulkan instance missing VK_KHR_win32_surface extension");
	//}


	memset(&sci, 0, sizeof(sci));
	sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	sci.hinstance = GetModuleHandle(NULL);
	sci.hwnd = w->Hwnd;

	auto err = vkCreateWin32SurfaceKHR(Instance, &sci, null, &surface);
	if(err)
	{
		throw CException(HERE, L"Failed to create Vulkan surface");
	}


	CDisplayDevice * dd = GetDeviceNearestTo(w->GetRect());

	auto d = new CVulkanDevice(Level, this, dd->Adapter, surface);
	
	Surfaces.push_back(surface);
	Devices.push_back(d);
	
	return d->AddTarget(w);
}
	
std::vector<const char*> CVulkanEngine::GetRequiredExtensions()
{
	std::vector<const char*> extensions(glfwExtensions);

	if(enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool CVulkanEngine::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for(const char* layerName : validationLayers) {
		bool layerFound = false;

		for(const auto& layerProperties : availableLayers) {
			if(strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if(!layerFound) {
			return false;
		}
	}

	return true;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	auto that = (CVulkanEngine *)pUserData;

	switch(messageSeverity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			that->Level->Log->ReportMessage(that, L"%s", CConverter::ToString(pCallbackData->pMessage));
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			that->Level->Log->ReportMessage(that, L"%s", CConverter::ToString(pCallbackData->pMessage));
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			that->Level->Log->ReportWarning(that, L"%s", CConverter::ToString(pCallbackData->pMessage));
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			that->Level->Log->ReportError(that, L"%s", CConverter::ToString(pCallbackData->pMessage));
			break;
	}
	
	return VK_FALSE;
}

VkResult CVulkanEngine::SetupDebugCallback()
{
	if(!enableValidationLayers)
		return VK_SUCCESS;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = this;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");

	if(func)
	{
		auto e = func(Instance, &createInfo, nullptr, &callback);
		return e;
	}
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;

	//if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to set up debug callback!");
	//}
}

void CVulkanEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
	
	if(func)
	{
		func(Instance, callback, pAllocator);
	}
}

#endif