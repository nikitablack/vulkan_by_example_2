#include "window/window.hpp"
#include "AppData.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>

bool framebufferResized{false};
bool solidMode{true};
float tesselationLevel{1.0};

void handle_error(std::exception const & error, uint32_t const level =  0)
{
    std::cout << std::string(level, ' ') << error.what() << '\n';

    try
    {
        std::rethrow_if_nested(error);
    }
    catch(std::exception const & e)
    {
        handle_error(error, level + 1);
    }
    catch(...)
    {
        std::cout << "unknown error" << '\n';
    }
}

int main()
{
    AppDataPtr appData{std::make_unique<AppData>()};

    try
    {
        appData = create_window(std::move(appData));
    }
    catch (AppDataError const & error)
    {
        handle_error(error);
        return 1;
    }
    
    while (!glfwWindowShouldClose(appData->window))
    {
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
}