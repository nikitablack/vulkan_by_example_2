#include "teapot_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <cassert>

MaybeAppDataPtr create_surface(AppDataPtr appData) noexcept
{
    assert(!appData->surface);
    
    if (glfwCreateWindowSurface(appData->instance, appData->window, nullptr, &appData->surface) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create window surface", std::move(appData)});
    
    return std::move(appData);
}