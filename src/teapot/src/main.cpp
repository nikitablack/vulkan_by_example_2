#include "window/teapot_window.h"
#include "AppData.h"
#include "Global.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>

bool framebufferResized{false};
bool solidMode{true};
float tesselationLevel{1.0};

int main()
{
    AppDataPtr appData{std::make_unique<AppData>()};
    
    auto mbAppData{create_window(std::move(appData))};
    
    if (!mbAppData)
    {
        std::cout << mbAppData.error() << std::endl;
        return 1;
    }
    
    appData = std::move(*mbAppData);
    
    while (!glfwWindowShouldClose(appData->window))
    {
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}