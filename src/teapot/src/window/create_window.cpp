#include "teapot_window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cassert>

MaybeAppDataPtr create_window(AppDataPtr appData) noexcept
{
    assert(!appData->window);
    
    if (!glfwInit())
        return tl::make_unexpected(AppDataError{"failed to init glfw", std::move(appData)});
    
    if (!glfwVulkanSupported())
        return tl::make_unexpected(AppDataError{"vulkan is not supported", std::move(appData)});
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
    GLFWwindow * const window{glfwCreateWindow(800, 600, "Teapot", nullptr, nullptr)};
    if (!window)
        return tl::make_unexpected(AppDataError{"failed to create window", std::move(appData)});
    
    appData->window = window;
    
    glfwSetKeyCallback(appData->window, &on_key_press);
    glfwSetWindowSizeLimits(appData->window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetFramebufferSizeCallback(appData->window, &framebuffer_size_callback);
    
    return std::move(appData);
}