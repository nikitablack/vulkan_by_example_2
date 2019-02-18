#include "clean.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

AppDataPtr clean(AppDataPtr appData) noexcept
{
    vkDestroyDevice(appData->device, nullptr);
    
    if(appData->instance)
        vkDestroySurfaceKHR(appData->instance, appData->surface, nullptr);
    
    //helpers::destroy_debug_utils_messenger(data->instance, data.debugUtilsMessenger);
    //vkDestroyInstance(data.instance, nullptr);
    glfwTerminate();
    
    return std::move(appData);
}