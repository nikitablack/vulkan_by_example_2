#include "utils/error_message.hpp"
#include "teapot_vulkan.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <cassert>

AppDataPtr create_surface(AppDataPtr appData)
{
    assert(!appData->surface);
    
    if (glfwCreateWindowSurface(appData->instance, appData->window, nullptr, &appData->surface) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create window surface"), std::move(*appData.release())};
    
    return appData;
}