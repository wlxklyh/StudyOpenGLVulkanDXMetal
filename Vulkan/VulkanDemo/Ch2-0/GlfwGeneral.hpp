#include "VKBase.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3.lib")

//窗口的指针 全局变量自动初始化为NULL
GLFWwindow* pWindow;
//显示器信息的指针
GLFWmonitor* pMonitor;
//窗口标题
const char* windowTitle = "Vulkan";


bool InitializeWindow(VkExtent2D size, bool fullScreen = false, bool isResizable = true
    , bool limitFrameRate = true)
{
    if(!glfwInit())
    {
        std::cout << std::format("[InitializeWindow] ERROR\nFailed to initialize GLFW. \n");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, isResizable);

    uint32_t extensionCount = 0;
    const char** extensionNames;
    extensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
    if(!extensionNames)
    {
        std::cout << std::format(("[Initializewindow]\nVulkan is not available on this machine!\n"));
        glfwTerminate();
        return false;
    }
    //VK_KHR_surface VK_KHR_win32_surface
    for (size_t i = 0; i < extensionCount;i ++)
        vulkan::graphicsBase::Base().AddInstanceExtension(extensionNames[i]);

    vulkan::graphicsBase::Base().AddDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    
    pMonitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);

    pWindow = fullScreen?
        glfwCreateWindow(pMode->width, pMode->height, windowTitle, pMonitor, nullptr):
        glfwCreateWindow(size.width, size.height, windowTitle, nullptr, nullptr);;
    
    if(!pWindow)
    {
        std::cout << std::format("[Initializewindow]\n Failed to create GLFW window. \n");
        return false;
    }
    vulkan::graphicsBase::Base().UseLatestApiVersion();
    
    vulkan::graphicsBase::Base().CreateInstance();

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (VkResult result = glfwCreateWindowSurface(vulkan::graphicsBase::Base().Instance(), pWindow, nullptr, &surface)) {
        std::cout << std::format("[ InitializeWindow ] ERROR\nFailed to create a window surface!\nError code: {}\n", int32_t(result));
        glfwTerminate();
        return false;
    }
    // graphicsBase::Base().Surface(surface);

    if (vulkan::graphicsBase::Base().GetPhysicalDevices() ||
        vulkan::graphicsBase::Base().DeterminePhysicalDevice(0, true, false) ||
        vulkan::graphicsBase::Base().CreateDevice())
        return false;
    return true;
}

bool TerminateWindow()
{
    glfwTerminate();
    return true;
}


void TileFps()
{
    static double time0 = glfwGetTime();
    static double time1;
    static double dt;
    static int dframe = -1;
    static std::stringstream info;
    time1 = glfwGetTime();

    dframe++;
    if((dt = time1 - time0) >= 1)
    {
        info.precision(1);
        info << windowTitle << " " << std::fixed << dframe / dt << " FPS";
        glfwSetWindowTitle(pWindow, info.str().c_str());
        info.str("");
        time0 = time1;
        dframe = 0;
    }
}

void MakeWindowFullScreen()
{
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, pMonitor, 0, 0, pMode->width, pMode->height, GLFW_DONT_CARE);
}

void MakeWindowWindowed(VkOffset2D position, VkExtent2D size)
{
    const GLFWvidmode* pMode = glfwGetVideoMode(pMonitor);
    glfwSetWindowMonitor(pWindow, nullptr, position.x, position.y, size.width, size.height, pMode->refreshRate);
    
}