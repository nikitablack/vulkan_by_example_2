#include "teapot_vulkan.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

void clean(AppData && appData) noexcept
{
    if (appData.device)
    {
        vkDestroyBuffer(appData.device, appData.projMatrixBuffer, nullptr);
        vkDestroyBuffer(appData.device, appData.viewMatrixBuffer, nullptr);
        vkDestroyBuffer(appData.device, appData.modelMatrixBuffer, nullptr);
        vkFreeMemory(appData.device, appData.matrixBuffersDeviceMemory, nullptr);
        
        vkDestroyBuffer(appData.device, appData.patchBuffer, nullptr);
        vkFreeMemory(appData.device, appData.patchBufferDeviceMemory, nullptr);
        
        vkDestroyBuffer(appData.device, appData.indexBuffer, nullptr);
        vkFreeMemory(appData.device, appData.indexBufferDeviceMemory, nullptr);
        
        vkDestroyBuffer(appData.device, appData.vertexBuffer, nullptr);
        vkFreeMemory(appData.device, appData.vertexBufferDeviceMemory, nullptr);
        
        vkDestroyShaderModule(appData.device, appData.vertexShaderModule, nullptr);
        vkDestroyShaderModule(appData.device, appData.tessControlShaderModule, nullptr);
        vkDestroyShaderModule(appData.device, appData.tessEvaluationShaderModule, nullptr);
        vkDestroyShaderModule(appData.device, appData.fragmentShaderModule, nullptr);
        
        vkDestroyDevice(appData.device, nullptr);
    }
    
    if (appData.instance)
    {
        vkDestroySurfaceKHR(appData.instance, appData.surface, nullptr);
    }
    
    vkDestroyInstance(appData.instance, nullptr);
    
    glfwTerminate();
}