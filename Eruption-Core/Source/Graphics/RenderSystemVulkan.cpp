#include "StdAfx.h"
#include "RenderSystemVulkan.h"

#include <algorithm>

namespace Eruption { namespace Graphics {

	void RenderSystemVulkan::Initialize(HWND windowHandle) {
		this->windowHandle = windowHandle;

		LOG_INFO("RenderSystemVulkan", "Initializing the Render System using the Vulkan Graphics Library...");
		LOG_INFO("RenderSystemVulkan", "Creating Vulkan Instance...");
		CreateInstance();
		//SetupDebugCallback(); // Throws exception!!
		CreateWindowSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();

	}

	void RenderSystemVulkan::CreateInstance() {
		// Fill some info structs for the instance
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_API_VERSION_1_0;
		appInfo.pApplicationName = "EruptionEngine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

		// specify required validation layers here
		vector<const char*> requiredLayers {
			"VK_LAYER_LUNARG_standard_validation"
		};

		// specify required vulkan extensions here
		vector<const char*> requiredExtensions{
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		};

		CheckLayerSupport(requiredLayers);			// Check for validation layer support
		CheckInstanceExtensionSupport(requiredExtensions);	// Check for vulkan extension support

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = vulkanInstance.layerCount;
		createInfo.enabledExtensionCount = vulkanInstance.extensionCount;
		createInfo.ppEnabledLayerNames = requiredLayers.data();
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		VULKAN_FUNC_CALL(vkCreateInstance(&createInfo, nullptr, vulkanInstance.instance.replace()), "Vulkan instance Created!", "Failed to create Vulkan instance!")
	}

	void RenderSystemVulkan::LoadVulkanExtensionFunctions() {
		vkCreateDebugReportCallbackEXT	= (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance.instance, "vkCreateDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vulkanInstance.instance, "vkDestroyDebugReportCallbackEXT");
	}

	void RenderSystemVulkan::SetupDebugCallback() {
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;

		VULKAN_FUNC_CALL(vkCreateDebugReportCallbackEXT(vulkanInstance.instance, &createInfo, nullptr, debugCallback.replace()), "Created Debug Callback!", "Failed to create Debug Callback!")
	}

	void RenderSystemVulkan::PickPhysicalDevice() {
		vulkanGPU.device = VK_NULL_HANDLE;

		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			LOG_ERROR("RenderSystemVulkan", "Failed to find GPU with Vulkan support!");
		}

		vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, devices.data());

		// Pick suitable device
		multimap<int, const VkPhysicalDevice&> ratedDevices;
		for (const VkPhysicalDevice& device : devices) {
			int rating = RatePhysicalDevice(device);
			if (rating == 0)
				continue;

			ratedDevices.insert(std::make_pair(rating, device));
		}

		vulkanGPU.device = ratedDevices.rbegin()->second;
		vulkanGPU.queueFamilyIndices = FindQueueFamilies(vulkanGPU.device);
		vulkanGPU.features = {};
		vulkanGPU.swapChainDetails = QuerySwapChainSupport();
	}

	void RenderSystemVulkan::CreateLogicalDevice() {
		vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		set<int> uniqueQueueFamilies = { vulkanGPU.queueFamilyIndices.graphicsFamily, vulkanGPU.queueFamilyIndices.presentFamily };

		float queuePriority = 1.0f;
		for (int queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vector<const char*> requiredExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		CheckDeviceExtensionSupport(requiredExtensions);

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32)queueCreateInfos.size();
		createInfo.pEnabledFeatures = &vulkanGPU.features;
		createInfo.enabledExtensionCount = vulkanGPU.extensionCount;
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		VULKAN_FUNC_CALL(vkCreateDevice(vulkanGPU.device, &createInfo, nullptr, vulkanDevice.device.replace()), "Created logical Device!", "Failed to create logical Device!")
		vkGetDeviceQueue(vulkanDevice.device, vulkanGPU.queueFamilyIndices.graphicsFamily, 0, &vulkanDevice.graphicsQueue);
		vkGetDeviceQueue(vulkanDevice.device, vulkanGPU.queueFamilyIndices.presentFamily, 0, &vulkanDevice.presentQueue);
	}

	void RenderSystemVulkan::CreateWindowSurface() {
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = windowHandle;
		createInfo.hinstance = GetModuleHandle(NULL);

		if (!vkCreateWin32SurfaceKHR) LOG_ERROR("RenderSystemVulkan", "Vulkan function vkCreateWin32SurfaceKHR not loaded!");
		VULKAN_FUNC_CALL(vkCreateWin32SurfaceKHR(vulkanInstance.instance, &createInfo, nullptr, vulkanSurface.replace()), "Created window surface!", "Failed to create window surface!");
	}

	void SwapChainSupportDetails::ChooseSurfaceFormat() {
		
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
			surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			return;
		}

		for (const VkSurfaceFormatKHR& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				surfaceFormat = format;
				return;
			}
		}

		surfaceFormat = formats[0];
	}

	void SwapChainSupportDetails::ChoosePresentMode() {
		for (const VkPresentModeKHR& mode : presentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				presentMode = mode;
				return;
			}
		}

		presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	void SwapChainSupportDetails::ChooseSwapExtent() {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32>::max()) {

		}

	}

	void RenderSystemVulkan::SetupSwapChain() {
		const SwapChainSupportDetails& swapChainDetails = vulkanGPU.swapChainDetails;

	}

	QueueFamilyIndices RenderSystemVulkan::FindQueueFamilies(const VkPhysicalDevice& device) {
		QueueFamilyIndices indices;

		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const VkQueueFamilyProperties& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vulkanSurface, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport)
				indices.presentFamily = i;

			if (indices.IsComplete())
				break;

			i++;
		}

		return indices;
	}

	SwapChainSupportDetails RenderSystemVulkan::QuerySwapChainSupport() {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanGPU.device, vulkanSurface, &details.capabilities);

		uint32 formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanGPU.device, vulkanSurface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanGPU.device, vulkanSurface, &formatCount, details.formats.data());
		}

		uint32 presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanGPU.device, vulkanSurface, &presentModeCount, nullptr);
		
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanGPU.device, vulkanSurface, &presentModeCount, details.presentModes.data());
		}



		return details;
	}

	int RenderSystemVulkan::RatePhysicalDevice(const VkPhysicalDevice& device) {
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		int rating = 0;
		if (!deviceFeatures.geometryShader)
			return 0;

		// Check for Queue Families
		QueueFamilyIndices indices = FindQueueFamilies(device);
		if (!indices.IsComplete())
			return 0;

		SwapChainSupportDetails swapChainDetails = QuerySwapChainSupport();
		if (swapChainDetails.formats.empty() && swapChainDetails.presentModes.empty())
			return 0;

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			rating += 1000;

		rating += deviceProperties.limits.maxImageDimension2D;

		return rating;
	}

	void RenderSystemVulkan::CheckInstanceExtensionSupport(const vector<const char*>& requiredExtensions) {
		LOG_INFO("RenderSystemVulkan", "Checking Instance Extension support...");
		VkResult result;

		uint32 count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

		VkExtensionProperties* extensions = NULL;

		do {
			if (extensions)
				free(extensions);

			extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * count);

			result = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions);
		} while (result == VK_INCOMPLETE);

		LOG_INFO("RenderSystemVulkan", "Available Instance Extensions: ");
		for (int i = 0; i < count; i++) {
			string logString = "Extension: " + string(extensions[i].extensionName);
			LOG_INFO("RenderSystemVulkan", logString);
		}
		LOG_NEW_LINE();

		for (const string& reqExt : requiredExtensions) {
			bool found = false;

			for (int i = 0; i < count; i++) {
				VkExtensionProperties extension = extensions[i];
				if (reqExt == extension.extensionName) {
					found = true;
					vulkanInstance.extensionCount++;
					vulkanInstance.extensionProperties.push_back(extension);
					LOG_SUCCESS("RenderSystemVulkan", "Extension " + reqExt + " found!");
					break;
				}
			}

			if (!found)
			{
				LOG_ERROR("RenderSystemVulkan", "Extenion " + reqExt + " is not supported!");
			}
		}

		LOG_NEW_LINE();
	}

	void RenderSystemVulkan::CheckLayerSupport(const vector<const char*>& requiredLayers) {
		LOG_INFO("RenderSystemVulkan", "Checking validation layer support...");
		VkResult result;

		uint32 count = 0;
		vkEnumerateInstanceLayerProperties(&count, nullptr);

		VkLayerProperties* layers = NULL;

		do {
			if (layers)
				free(layers);

			layers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * count);

			result = vkEnumerateInstanceLayerProperties(&count, layers);
		} while (result == VK_INCOMPLETE);

		LOG_INFO("RenderSystemVulkan", "Available Layers: ");
		for (int i = 0; i < count; i++) {
			string logString = "Layer: " + string(layers[i].layerName);
			LOG_INFO("RenderSystemVulkan", logString);
		}
		LOG_NEW_LINE();

		for (const string& reqLayer : requiredLayers) {
			bool found = false;

			for (int i = 0; i < count; i++) {
				VkLayerProperties layer = layers[i];
				if (reqLayer == layer.layerName) {
					found = true;
					vulkanInstance.layerCount++;
					vulkanInstance.layerProperties.push_back(layer);
					LOG_SUCCESS("RenderSystemVulkan", "Layer " + reqLayer + " was found!");
					break;
				}
			}

			if (!found)
			{
				LOG_ERROR("RenderSystemVulkan", "Layer " + reqLayer + " is not supported!");
			}
		}

		LOG_NEW_LINE();
	}

	void RenderSystemVulkan::CheckDeviceExtensionSupport(const vector<const char*> requiredExtensions) {
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties(vulkanGPU.device, nullptr, &extensionCount, nullptr);

		vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(vulkanGPU.device, nullptr, &extensionCount, availableExtensions.data());
		
		for (const string& reqExt : requiredExtensions) {
			bool found = false;

			for (const VkExtensionProperties& extension : availableExtensions) {
				if (reqExt == extension.extensionName) {
					found = true;
					vulkanGPU.extensionCount++;
					vulkanGPU.extensionProperties.push_back(extension);
					LOG_SUCCESS("RenderSystemVulkan", "Extension " + reqExt + " was found!");
					break;
				}
			}
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL RenderSystemVulkan::DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64 obj, size_t location, int32 code, const char* layerPrefix, const char* msg, void* userData)
	{
		switch (flags) {
		case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
		case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
			LOG_INFO("VulkanMessage", string(msg));
			break;
		case VK_DEBUG_REPORT_WARNING_BIT_EXT:
		case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
			LOG_WARNING("VulkanMessage", string(msg));
			break;
		case VK_DEBUG_REPORT_ERROR_BIT_EXT:
			LOG_ERROR("VulkanMessage", string(msg));
			break;
		}

		return VK_FALSE;
	}

} }