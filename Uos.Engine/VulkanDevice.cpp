#include "StdAfx.h"
#include "VulkanDevice.h"
#include "VulkanEngine.h"

#ifdef UOS_ENGINE_VULKAN

using namespace uos;

CVulkanDevice::CVulkanDevice(CEngineLevel * l, CVulkanEngine * ge, CDisplayAdapter * a, VkSurfaceKHR s) : CEngineEntity(l)
{
	Engine	= ge;
	Surface = s;

	auto g = l->Config->Root->One(L"GraphicEngine/GraphicDriver");

	VSync	= g->One(L"IsVSync")->AsBool();

	for(auto i : ge->PhysicalDevices)
	{
		VkPhysicalDeviceIDProperties p;
		p.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
		p.pNext = 0;

		VkPhysicalDeviceProperties2 p2;
		p2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		p2.pNext = &p;


		vkGetPhysicalDeviceProperties2(i, &p2);

		//uint32_t n = 3;
		//VkDisplayPropertiesKHR d[3];
		//auto e = vkGetPhysicalDeviceDisplayPropertiesKHR(physicalDevice, &n, d);


		if(p.deviceLUIDValid && memcmp(p.deviceLUID, &a->Luid, sizeof(p.deviceLUID)) == 0)
		{
			PhysicalDevice = i;
		}
	}

	QueueFamilyIndices indices = findQueueFamilies();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

	float queuePriority = 1.0f;
	for(int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(Extensions.size());
	createInfo.ppEnabledExtensionNames = Extensions.data();

	if(ge->enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(ge->validationLayers.size());
		createInfo.ppEnabledLayerNames = ge->validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if(vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) != VK_SUCCESS)
	{
		throw CException(HERE, L"Failed to create logical device");
	}

	vkGetDeviceQueue(Device, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(Device, indices.presentFamily, 0, &presentQueue);

	//CGraphicTarget * t = new CGraphicTarget(Level, this);
	//
	//InitFullscreenPresentParameters(t, dd);
	//
	//HRESULT hr = Engine->D3D->CreateDevice(dd->Dx9Index, D3DDEVTYPE_HAL, GetDesktopWindow(), GetDeviceFlags(dd), &t->PresentParameters, &D3DDevice);
	//
	//if(hr == S_OK)
	//{
	//	InitCommon(t);
	//	Targets.push_back(t);
	//	Level->Log->ReportMessage(this, L"Created as %s", dd->Name.c_str());
	//}
	//else
	//	throw CAttentionException(HERE, L"Could not create 3d device. 3d device is not available.");
}

CVulkanDevice::~CVulkanDevice()
{
	for(auto i : Targets)
	{
		delete i;
	}

	Level->Level0->LevelCreated		-= ThisHandler(OnLevelCreated); 
	Level->Core->ExitQueried		-= ThisHandler(OnLevel1ExitQueried);
	Level->Core->Suspended			-= ThisHandler(OnLevel1SuspendingStarted);
}

QueueFamilyIndices CVulkanDevice::findQueueFamilies()
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for(const auto& queueFamily : queueFamilies)
	{
		if(queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &presentSupport);

		if(queueFamily.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}

		if(indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails CVulkanDevice::querySwapChainSupport()
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &formatCount, nullptr);

	if(formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &presentModeCount, nullptr);

	if(presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool CVulkanDevice::isDeviceSuitable()
{
	QueueFamilyIndices indices = findQueueFamilies();

	bool extensionsSupported = checkDeviceExtensionSupport();

	bool swapChainAdequate = false;
	if(extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport();
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool CVulkanDevice::checkDeviceExtensionSupport()
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(Extensions.begin(), Extensions.end());

	for(const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}


void CVulkanDevice::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	if (vkCreateCommandPool(Device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}

CScreenRenderTarget * CVulkanDevice::GetPrimaryTarget()
{
	return Targets[0];
}

CScreenRenderTarget * CVulkanDevice::AddTarget(CWindowScreen * s)
{
	auto t = new CVulkanScreenTarget(Level, this, s);

	return t;
}

void CVulkanDevice::RemoveTarget(CScreenRenderTarget * t)
{
	Targets.Remove(t);
	delete t;
}

void CVulkanDevice::OnLevel1ExitQueried()
{
	SwitchToGDI();
}

void CVulkanDevice::OnLevel1SuspendingStarted()
{
	SwitchToGDI();
}

void CVulkanDevice::OnLevelCreated(int n, ILevel * l)
{
}

void CVulkanDevice::Present()
{
}

void CVulkanDevice::TakeScreenShot(CScreenRenderTarget * t, CUol & p)
{
}

#endif