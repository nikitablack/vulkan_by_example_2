#include "teapot_vulkan.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

AppDataPtr get_required_window_extensions(AppDataPtr appData) noexcept
{
    uint32_t glfwExtensionCount{0};
    char const * const * const glfwExtensions{glfwGetRequiredInstanceExtensions(&glfwExtensionCount)};
    
    for (uint32_t i{0}; i < glfwExtensionCount; ++i)
        appData->instanceExtensions.push_back(glfwExtensions[i]);
    
    return appData;
}