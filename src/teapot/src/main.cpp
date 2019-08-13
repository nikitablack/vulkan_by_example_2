#include "vulkan/teapot_vulkan.hpp"
#include "window/window.hpp"
#include "AppData.hpp"
#include "Global.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>

bool framebufferResized{false};
bool solidMode{true};
float tesselationLevel{1.0};

static_assert(sizeof(float) == shaderDataSize, "expected float to be 4 bytes");

void handle_error(std::exception const & error, uint32_t const level =  0)
{
    std::cout << std::string(level, ' ') << error.what() << '\n';
    
    try
    {
        std::rethrow_if_nested(error);
    }
    catch(std::exception const & nestedError)
    {
        handle_error(nestedError, level + 1);
    }
    catch(...)
    {
        std::cout << "unknown error" << '\n';
    }
}

int main()
{
    AppDataPtr appData{std::make_unique<AppData>()};
    
    appData->layers.push_back("VK_LAYER_KHRONOS_validation");
    appData->layers.push_back("VK_LAYER_LUNARG_monitor");

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    appData->instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    
    try
    {
        appData = create_window(std::move(appData));
        appData = get_required_window_extensions(std::move(appData));
        appData = create_instance(std::move(appData));
        appData = create_surface(std::move(appData));
        appData = get_physical_device(std::move(appData));
        appData = create_logical_device(std::move(appData));
        appData = create_shader_modules(std::move(appData));
        appData = get_device_qeues(std::move(appData));
        appData = create_vertex_buffer(std::move(appData));
        appData = create_index_buffer(std::move(appData));
        appData = create_patch_buffer(std::move(appData));
        appData = create_matrix_buffers(std::move(appData));
        appData = create_descriptor_set_layout(std::move(appData));
        appData = create_descriptor_pool(std::move(appData));
        appData = allocate_and_update_descriptor_sets(std::move(appData));
        appData = create_pipeline_layout(std::move(appData));
        appData = create_render_pass(std::move(appData));
        appData = create_pipelines(std::move(appData));
    }
    catch (AppDataError & error)
    {
        handle_error(error);
        
        clean(std::move(error.appData));
        
        return 1;
    }
    catch (...)
    {
        std::cout << "unknown error" << '\n';
        return 1;
    }
    
    while (!glfwWindowShouldClose(appData->window))
    {
        glfwPollEvents();
    }
    
    auto appDataCopy{*appData.release()};
    clean(appDataCopy);
    
    return 0;
}