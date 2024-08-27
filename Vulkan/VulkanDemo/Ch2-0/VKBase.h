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
        {
        }

        //(1)实例
        VkInstance instance;

        //(2)layers和扩展
        std::vector<const char*> instanceLayers;
        std::vector<const char*> instanceExtensions;

        static void AddLayerOrExtension(std::vector<const char*>& container, const char* name)
        {
            for (auto& i : container)
            {
                if (!strcmp(name, i))
                {
                    return;
                }
            }
            container.push_back(name);
        }

        //(3)调试
        VkDebugUtilsMessengerEXT debugMessenger;

        VkResult CreateDebugMessenger()
        {
            static PFN_vkDebugUtilsMessengerCallbackEXT DebugUtilsMessengerCallback = [](
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData)-> VkBool32
            {
                std::cout << std::format("{}\n\n", pCallbackData->pMessage);
                return VK_FALSE;
            };

            VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = DebugUtilsMessengerCallback
            };
            PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger =
                reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
                    instance, "vkCreateDebugUtilsMessengerEXT"));
            if (vkCreateDebugUtilsMessenger)
            {
                VkResult result = vkCreateDebugUtilsMessenger(instance, &debugUtilsMessengerCreateInfo, nullptr,
                                                              &debugMessenger);
                if (result)
                    std::cout << std::format(
                        "[ graphicsBase ] ERROR\nFailed to create a debug messenger!\nError code: {}\n",
                        int32_t(result));
                return result;
            }
            std::cout << std::format(
                "[ graphicsBase ] ERROR\nFailed to get the function pointer of vkCreateDebugUtilsMessengerEXT!\n");
            return VK_RESULT_MAX_ENUM;
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

        VkResult GetQueueFamilyIndices(VkPhysicalDevice physicalDevice, bool enableGraphicsQueue,
                                       bool enableComputeQueue, uint32_t (&queueFamilyIndices)[3])
        {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
            if (!queueFamilyCount)
            {
                return VK_RESULT_MAX_ENUM;
            }
            std::vector<VkQueueFamilyProperties> queueFamilyPropertieses(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyPropertieses.data());

            auto& [ig, ip, ic] = queueFamilyIndices;
            ig = ip = ic = VK_QUEUE_FAMILY_IGNORED;
            for (uint32_t i = 0; i < queueFamilyCount; i++)
            {
                //这三个VkBool32变量指示是否可获取（指应该被获取且能获取）相应队列族索引
                VkBool32
                    //只在enableGraphicsQueue为true时获取支持图形操作的队列族的索引
                    supportGraphics = enableGraphicsQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_GRAPHICS_BIT,
                    supportPresentation = false,
                    //只在enableComputeQueue为true时获取支持计算的队列族的索引
                    supportCompute = enableComputeQueue && queueFamilyPropertieses[i].queueFlags & VK_QUEUE_COMPUTE_BIT;
                //只在创建了window surface时获取支持呈现的队列族的索引
                if (surface)
                {
                    if (VkResult result = vkGetPhysicalDeviceSurfaceSupportKHR(
                        physicalDevice, i, surface, &supportPresentation))
                    {
                        std::cout << std::format(
                            "[ graphicsBase ] ERROR\nFailed to determine if the queue family supports presentation!\nError code: {}\n",
                            int32_t(result));
                        return result;
                    }
                }
   
                //若某队列族同时支持图形操作和计算
                if (supportGraphics && supportCompute)
                {
                    //若需要呈现，最好是三个队列族索引全部相同
                    if (supportPresentation)
                    {
                        ig = ip = ic = i;
                        break;
                    }
                    //除非ig和ic都已取得且相同，否则将它们的值覆写为i，以确保两个队列族索引相同
                    if (ig != ic ||
                        ig == VK_QUEUE_FAMILY_IGNORED)
                        ig = ic = i;
                    //如果不需要呈现，那么已经可以break了
                    if (!surface)
                        break;
                }
                //若任何一个队列族索引可以被取得但尚未被取得，将其值覆写为i
                if (supportGraphics &&
                    ig == VK_QUEUE_FAMILY_IGNORED)
                    ig = i;
                if (supportPresentation &&
                    ip == VK_QUEUE_FAMILY_IGNORED)
                    ip = i;
                if (supportCompute &&
                    ic == VK_QUEUE_FAMILY_IGNORED)
                    ic = i;
            }
            if (ig == VK_QUEUE_FAMILY_IGNORED && enableGraphicsQueue ||
                ip == VK_QUEUE_FAMILY_IGNORED && surface ||
                ic == VK_QUEUE_FAMILY_IGNORED && enableComputeQueue)
                return VK_RESULT_MAX_ENUM;
            queueFamilyIndex_graphics = ig;
            queueFamilyIndex_presentation = ip;
            queueFamilyIndex_compute = ic;
            return VK_SUCCESS;
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
        VkResult UseLatestApiVersion()
        {
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
            std::cout << "======Layers:" << std::endl;
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<VkLayerProperties> layerProperties(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
            for (uint32_t i = 0; i < layerProperties.size(); i++)
            {
                std::cout << layerProperties[i].layerName << std::endl;
            }
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

            if (VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance))
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
            uint32_t layerCount;
            std::vector<VkLayerProperties> availableLayers;
            if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr))
            {
                std::cout << std::format("[ graphicsBase ] ERROR\nFailed to get the count of instance layers!\n");
                return result;
            }

            if (layerCount)
            {
                availableLayers.resize(layerCount);
                if (VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()))
                {
                    std::cout << std::format(
                        "[ graphicsBase ] ERROR\nFailed to enumerate instance layer properties!\nError code: {}\n",
                        int32_t(result));
                    return result;
                }
                for (auto& i : layersToCheck)
                {
                    bool found = false;
                    for (auto& j : availableLayers)
                        if (!strcmp(i, j.layerName))
                        {
                            found = true;
                            break;
                        }
                    if (!found)
                        i = nullptr;
                }
            }
            else
                for (auto& i : layersToCheck)
                    i = nullptr;
            //一切顺利则返回VK_SUCCESS
            return VK_SUCCESS;
        }

        void InstanceLayers(const std::vector<const char*>& layerNames)
        {
            instanceLayers = layerNames;
        }

        VkResult CheckInstanceExtensions(std::span<const char*> extensionsToCheck,
                                         const char* layerName = nullptr) const
        {
            uint32_t extensionCount;
            std::vector<VkExtensionProperties> availableExtensions;
            if (VkResult result = vkEnumerateInstanceExtensionProperties(layerName, &extensionCount, nullptr))
            {
                layerName
                    ? std::cout << std::format(
                        "[ graphicsBase ] ERROR\nFailed to get the count of instance extensions!\nLayer name:{}\n",
                        layerName)
                    : std::cout << std::format(
                        "[ graphicsBase ] ERROR\nFailed to get the count of instance extensions!\n");
                return result;
            }
            if (extensionCount)
            {
                availableExtensions.resize(extensionCount);
                if (VkResult result = vkEnumerateInstanceExtensionProperties(
                    layerName, &extensionCount, availableExtensions.data()))
                {
                    std::cout << std::format(
                        "[ graphicsBase ] ERROR\nFailed to enumerate instance extension properties!\nError code: {}\n",
                        int32_t(result));
                    return result;
                }
                for (auto& i : extensionsToCheck)
                {
                    bool found = false;
                    for (auto& j : availableExtensions)
                        if (!strcmp(i, j.extensionName))
                        {
                            found = true;
                            break;
                        }
                    if (!found)
                        i = nullptr;
                }
            }
            else
                for (auto& i : extensionsToCheck)
                    i = nullptr;
            return VK_SUCCESS;
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
            if (!this->surface)
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
            uint32_t deviceCount;
            if (VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr))
            {
                std::cout << std::format(
                    "[ graphicsBase ] ERROR\nFailed to get the count of physical devices!\nError code: {}\n",
                    int32_t(result));
                return result;
            }
            if (!deviceCount)
            {
                std::cout << std::format(
                        "[ graphicsBase ] ERROR\nFailed to find any physical device supports vulkan!\n"),
                    abort();
            }
            availablePhysicalDevices.resize(deviceCount);
            VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, availablePhysicalDevices.data());
            if (result)
                std::cout << std::format(
                    "[ graphicsBase ] ERROR\nFailed to enumerate physical devices!\nError code: {}\n", int32_t(result));

            std::cout << "======devices:" << std::endl;
            for (VkPhysicalDevice& device : availablePhysicalDevices)
            {
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);
                std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
                std::cout << "Device Type: " << deviceProperties.deviceType << std::endl;
                std::cout << "Driver Version: " << deviceProperties.driverVersion << std::endl;
            }
            return result;
        }

        VkResult DeterminePhysicalDevice(uint32_t deviceIndex = 0, bool enableGraphicsQueue = true,
                                         bool enableComputeQueue = true)
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

        uint32_t AvailableSurfaceFormatCount() const
        {
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
