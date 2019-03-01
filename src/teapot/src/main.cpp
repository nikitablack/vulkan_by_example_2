#include "vulkan/teapot_vulkan.h"
#include "window/teapot_window.h"
#include "AppData.h"
#include "Global.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>

bool framebufferResized{false};
bool solidMode{true};
float tesselationLevel{1.0};
uint32_t numConcurrentResources{2};

static_assert(sizeof(float) == shaderDataSize, "expected float to be 4 bytes");

int main()
{
    AppDataPtr appData{std::make_unique<AppData>()};
    
    appData->layers.push_back("VK_LAYER_LUNARG_standard_validation");
    appData->deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    appData->instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    
    appData->debugCallback = &vulkan_debug_callback;
    
    auto mbAppData{create_window(std::move(appData))
                   .map(get_required_window_extensions)
                   .and_then(create_instance)
                   .and_then(create_debug_utils_messenger)
                   .and_then(create_surface)
                   .and_then(get_physical_device)
                   .and_then(create_logical_device)
                   .and_then(create_shader_modules)
                   .map(get_device_qeues)
                   .and_then(create_vertex_buffer)
                   .and_then(create_index_buffer)
                   .and_then(create_patch_buffer)
                   .and_then(create_matrix_buffers)
                   .and_then(create_descriptor_set_layout)
                   .and_then(create_descriptor_pool)
                   .and_then(allocate_and_update_descriptor_sets)
                   .and_then(create_pipeline_layout)
                   .and_then(create_render_pass)
                   .and_then(create_pipelines)
                   .and_then(create_swap_chain)
                   .and_then(get_swapchain_images_and_views)
                   .and_then(create_framebuffers)};
    
    if (!mbAppData)
    {
        std::cout << mbAppData.error().message << std::endl;
    
        appData = clean(std::move(appData));
        
        return 1;
    }
    
    appData = std::move(*mbAppData);
    
    while (!glfwWindowShouldClose(appData->window))
    {
        glfwPollEvents();
    }
    
    appData = clean(std::move(appData));
    
    return 0;
}