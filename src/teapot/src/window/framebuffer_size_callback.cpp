#include "teapot_window.h"
#include "Global.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow * const /*window*/, int const /*width*/, int const /*height*/) noexcept
{
    framebufferResized = true;
}