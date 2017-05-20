#pragma once
#include <Vulkan\vulkan.h>

#include "Utility\VObject.h"

namespace Eruption { namespace Graphics {

	using Utility::VObject;

	struct QueueFamilyIndices {
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool IsComplete() { return graphicsFamily >= 0 && presentFamily >= 0; }
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;

		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
		VkExtent2D swapExtent2D;

		vector<VkSurfaceFormatKHR> formats;
		vector<VkPresentModeKHR> presentModes;

		void ChooseSurfaceFormat();
		void ChoosePresentMode();
		void ChooseSwapExtent();
	};

	class RenderSystemVulkan {
	
	private:

		struct VulkanInstance {
			VObject<VkInstance> instance{ vkDestroyInstance };

			uint32 layerCount;
			uint32 extensionCount;

			vector<VkLayerProperties> layerProperties;
			vector<VkExtensionProperties> extensionProperties;
		};

		struct VulkanGPU {
			uint32 id;
			VkPhysicalDevice device;

			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;

			QueueFamilyIndices queueFamilyIndices;
			SwapChainSupportDetails swapChainDetails;

			uint32 extensionCount;
			vector<VkExtensionProperties> extensionProperties;
		};

		struct VulkanDevice {
			VObject<VkDevice> device{ vkDestroyDevice };

			VkQueue graphicsQueue;
			VkQueue presentQueue;
		};

	private:
		HWND windowHandle;
		VulkanInstance	vulkanInstance;
		VulkanGPU		vulkanGPU;
		VulkanDevice	vulkanDevice;
		VObject<VkSurfaceKHR> vulkanSurface{ vulkanInstance.instance, vkDestroySurfaceKHR };

		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
		PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT;
		
		VObject<VkDebugReportCallbackEXT> debugCallback{vulkanInstance.instance, vkDestroyDebugReportCallbackEXT };


	public:

		void Initialize(HWND windowHandle);

	private:

		// Initialization
		void CreateInstance();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateWindowSurface();
		void SetupSwapChain();

		void LoadVulkanExtensionFunctions();

		int RatePhysicalDevice(const VkPhysicalDevice& device);

		QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
		SwapChainSupportDetails QuerySwapChainSupport();

		// Debug Stuff
		void CheckLayerSupport(const vector<const char*>& requiredLayers);
		void CheckInstanceExtensionSupport(const vector<const char*>& requiredExtensions);
		void CheckDeviceExtensionSupport(const vector<const char*> requiredExtensions);
		void SetupDebugCallback();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64 obj, size_t location, int32 code, const char* layerPrefix, const char* msg, void* userData);
	};

} }