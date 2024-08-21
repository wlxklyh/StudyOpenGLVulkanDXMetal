#pragma once

#include "EasyVKStart.h"

namespace vulkan
{
    class graphicsBase
    {
        uint32_t apiVersion = VK_API_VERSION_1_0;
        //(0)单例
        static graphicsBase singleton;
        graphicsBase() = default;
        graphicsBase(graphicsBase&&) = delete;
        ~graphicsBase()
        {}

        //(1)实例
        VkInstance instance;

        //(2)layers和扩展
        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;
        static void AddLayerOrExtension(std::vector<const char*>& container, const char* name)
        {
            for(auto& i : container)
            {
                if (!strcmp(name, i))
                {
                    return;
                }
                container.push_back(name);
            }
        }
        
        //(3)调试
        VkDebugUtilsMessengerEXT debugMessenger;
        VkResult CreateDebugMessenger()
        {
            
        }

        //(4)surface
        VkSurfaceKHR surface;

        //(5)物理设备 逻辑设备
        VkPhysicalDevice physicalDevice;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        std::vector<VkPhysicalDevice> availablePhysicalDevices;
        VkDevice device;
        uint32_t queueFamilyIndex_graphics = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueFamilyIndex_presentation = VK_QUEUE_FAMILY_IGNORED;
        uint32_t queueFamilyIndex_compute = VK_QUEUE_FAMILY_IGNORED;

        VkQueue queue_graphics;
        VkQueue queue_presentation;
        VkQueue queue_compute;
        std::vector<const char*> deviceExtensions;
        VkResult GetQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool enableGraphicsQueue, bool enableComputeQueue, uint32_t (&queueFamilyIndices)[3]) {
            
        }

        //(6)交换链
        std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;

        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};

        VkResult CreateSwapchain_Internal()
        {
            
        }

        
    public:
        VkResult UseLatestApiVersion() {
            if (vkGetInstanceProcAddr(VK_NULL_HANDLE, "vkEnumerateInstanceVersion"))
                return vkEnumerateInstanceVersion(&apiVersion);
            return VK_SUCCESS;
        }
        
        //(0)单例
        static graphicsBase& Base()
        {
            return singleton;
        }
        VkInstance Instance() const
        {
            return instance;
        }
        
        //(1)实例
        VkResult CreateInstance(VkInstanceCreateFlags flags = 0)
        {
#ifndef NDEBUG
            AddInstanceLayer("VK_LAYER_KHRONOS_validation");
            AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
            VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .apiVersion = apiVersion
            };
            VkInstanceCreateInfo instanceCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .flags = flags,
                .pApplicationInfo = &applicationInfo,
                .enabledLayerCount = uint32_t(instanceLayers.size()),
                .ppEnabledLayerNames = instanceLayers.data(),
                .enabledExtensionCount = uint32_t(instanceExtensions.size()),
                .ppEnabledExtensionNames = instanceExtensions.data(),
            };

            if(VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance))
            {
                std::cout << std::format("[graphicsBase]Error\nFailed to create an instance!\n");
                return result;
            }
            std::cout << std::format(
                "Vulkan API Version:{}.{}.{}\n",
                VK_VERSION_MAJOR(apiVersion),
                VK_VERSION_MINOR(apiVersion),
                VK_VERSION_PATCH(apiVersion)
                );

#ifndef NDEBUG
            CreateDebugMessenger();
#endif
            return VK_SUCCESS;
        }
        
        //(2)layer extension
        const std::vector<const char*>& InstanceLayers() const
        {
            return instanceLayers;
        }
        const std::vector<const char*>& InstanceExtensions() const
        {
            return instanceExtensions;
        }
        void AddInstanceLayer(const char* layerName)
        {
            AddLayerOrExtension(instanceLayers, layerName);
        }
        void AddInstanceExtension(const char* extensionName)
        {
            AddLayerOrExtension(instanceExtensions, extensionName);
        }
        VkResult CheckInstanceLayers(std::span<const char*> layersToCheck)
        {
            
        }
        void InstanceLayers(const std::vector<const char*>& layerNames)
        {
            instanceLayers = layerNames;
        }
        VkResult CheckInstanceExtensions(std::span<const char*> extensionsToCheck, const char* layerName = nullptr) const 
        {
            
        }
        void InstanceExtensions(const std::vector<const char*>& extensionNames)
        {
            instanceExtensions = extensionNames;
        }
        
        //(4)surface
        VkSurfaceKHR Surface() const
        {
            return surface;
        }
        void Surface(VkSurfaceKHR surface)
        {
            if(!this->surface)
            {
                this->surface = surface;
            }
        }
        
        //(5)物理设备 逻辑设备
        VkPhysicalDevice PhysicalDevice() const
        {
            return physicalDevice;
        }
        const VkPhysicalDeviceProperties& PhysicalDeviceProperties() const
        {
            return physicalDeviceProperties;
        }
        const VkPhysicalDeviceMemoryProperties& PhysicalDeviceMemoryProperties() const
        {
            return physicalDeviceMemoryProperties;
        }
        VkPhysicalDevice AvailablePhysicalDevice(uint32_t index) const
        {
            return availablePhysicalDevices[index];
        }
        uint32_t AvailablePhysicalDeviceCount() const
        {
            return uint32_t(availablePhysicalDevices.size());
        }
        VkDevice Device()
        {
            return device;
        }
        uint32_t QueueFamilyIndex_Graphics() const
        {
            return queueFamilyIndex_graphics;
        }
        uint32_t QueueFamilyIndex_Presentation() const
        {
            return queueFamilyIndex_presentation;
        }
        uint32_t QueueFamilyIndex_Compute() const
        {
            return queueFamilyIndex_compute;
        }
        VkQueue Queue_Graphics()
        {
            return queue_graphics;
        }
        VkQueue Queue_Presentation()
        {
            return queue_presentation;
        }
        VkQueue Queue_Compute()
        {
            return queue_compute;
        }
        const std::vector<const char*>& DeviceExtensions() const
        {
            return deviceExtensions;
        }

        void AddDeviceExtension(const char* extensionName)
        {
            AddLayerOrExtension(deviceExtensions, extensionName);
        }
        VkResult GetPhysicalDevices()
        {
            
        }
        VkResult DeterminePhysicalDevice(uint32_t deviceIndex = 0, bool enableGraphicsQueue=true, bool enableComputeQueue=true)
        {
            
        }
        VkResult CreateDevice(VkDeviceCreateFlags flags = 0)
        {
            
        }
        VkResult CheckDeviceExtensions(std::span<const char*> extensionsToCheck, const char* layerName = nullptr) const
        {
            
        }
        void DeviceExtension(const std::vector<const char*>& extensionsNames)
        {
            deviceExtensions = extensionsNames;
        }
        
        //(6)交换链
        const VkFormat& AvailableSurfaceFormat(uint32_t index) const
        {
            return availableSurfaceFormats[index].format;
        }
        const VkColorSpaceKHR& AvailableSurfaceColorSpace(uint32_t index)
        {
            return availableSurfaceFormats[index].colorSpace;
        }
        uint32_t AvailableSurfaceFormatCount() const {
            return uint32_t(availableSurfaceFormats.size());
        }
        VkSwapchainKHR SwapChain() const
        {
            return swapchain;
        }
        VkImage SwapchainImage(uint32_t index) const
        {
            return swapchainImages[index];
        }
        VkImageView SwapchainImageView(uint32_t index) const
        {
            return swapchainImageViews[index];
        }
        uint32_t SwapchainImageCount() const
        {
            return swapchainImages.size();
        }
        const VkSwapchainCreateInfoKHR& SwapchainCreateInfo() const
        {
            return swapchainCreateInfo;
        }
        VkResult GetSurfaceFormats()
        {
            
        }
        VkResult SetSurfaceFormat(VkSurfaceFormatKHR surfaceFormat)
        {
            
        }
        VkResult CreateSwapchain(bool limitFrameRate = true, VkSwapchainCreateFlagsKHR flags = 0)
        {
            
        }
        VkResult RecreateSwapchain()
        {
            
        }
    };

    inline graphicsBase graphicsBase::singleton;
}
