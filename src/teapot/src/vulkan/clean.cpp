#include "teapot_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

AppDataPtr clean(AppDataPtr appData) noexcept
{
    vkDestroyShaderModule(appData->device, appData->vertexShaderModule, nullptr);
    appData->vertexShaderModule = VK_NULL_HANDLE;
    
    vkDestroyShaderModule(appData->device, appData->tessControlShaderModule, nullptr);
    appData->tessControlShaderModule = VK_NULL_HANDLE;
    
    vkDestroyShaderModule(appData->device, appData->tessEvaluationShaderModule, nullptr);
    appData->tessEvaluationShaderModule = VK_NULL_HANDLE;
    
    vkDestroyShaderModule(appData->device, appData->fragmentShaderModule, nullptr);
    appData->fragmentShaderModule = VK_NULL_HANDLE;
    
    vkDestroyDevice(appData->device, nullptr);
    appData->device = VK_NULL_HANDLE;
    
    if (appData->instance)
    {
        vkDestroySurfaceKHR(appData->instance, appData->surface, nullptr);
        appData->surface = VK_NULL_HANDLE;
    }
    
    {
        PFN_vkDestroyDebugUtilsMessengerEXT const func{
                reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(appData->instance,
                                                                                            "vkDestroyDebugUtilsMessengerEXT"))};
        
        if (func)
        {
            func(appData->instance, appData->debugUtilsMessenger, nullptr);
            appData->debugUtilsMessenger = VK_NULL_HANDLE;
        }
    }
    
    vkDestroyInstance(appData->instance, nullptr);
    appData->instance = VK_NULL_HANDLE;
    
    glfwTerminate();
    
    return std::move(appData);
}