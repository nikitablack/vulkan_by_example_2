#include "teapot_window.h"
#include "Global.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>

void on_key_press(GLFWwindow * const /*window*/, int const key, int const /*scancode*/, int const action,
                  int const /*mods*/) noexcept
{
    if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        tesselationLevel = std::min(tesselationLevel + 0.1f, 64.0f);
    }
    else if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        tesselationLevel = std::max(tesselationLevel - 0.1f, 64.0f);
    }
    else if (key == GLFW_KEY_SPACE && (action == GLFW_RELEASE))
    {
        solidMode = !solidMode;
    }
}