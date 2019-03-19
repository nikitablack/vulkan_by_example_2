#include "teapot_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

AppDataPtr clean(AppDataPtr appData) noexcept
{
    if (appData->device)
    {
        for (auto const framebuffer : appData->framebuffers)
            vkDestroyFramebuffer(appData->device, framebuffer, nullptr);
        
        appData->framebuffers.clear();
        
        for (auto const imageView : appData->swapchainImageViews)
            vkDestroyImageView(appData->device, imageView, nullptr);
        
        appData->swapchainImageViews.clear();
        
        auto const destroySwapChainFunc{ reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetInstanceProcAddr(appData->instance, "vkDestroySwapchainKHR")) };
        
        if (destroySwapChainFunc)
        {
            destroySwapChainFunc(appData->device, appData->swapchain, nullptr);
            appData->swapchain = VK_NULL_HANDLE;
        }
        
        appData->swapchainImages.clear();
        
        vkDestroyRenderPass(appData->device, appData->renderPass, nullptr);
        appData->renderPass = VK_NULL_HANDLE;
        
        vkDestroyPipelineLayout(appData->device, appData->pipelineLayout, nullptr);
        appData->pipelineLayout = VK_NULL_HANDLE;
        
        vkDestroyPipeline(appData->device, appData->solidPipeline, nullptr);
        appData->solidPipeline = VK_NULL_HANDLE;
        
        vkDestroyPipeline(appData->device, appData->wireframePipeline, nullptr);
        appData->wireframePipeline = VK_NULL_HANDLE;
        
        vkDestroyDescriptorSetLayout(appData->device, appData->descriptorSetLayout, nullptr);
        appData->descriptorSetLayout = VK_NULL_HANDLE;
        
        vkDestroyDescriptorPool(appData->device, appData->descriptorPool, nullptr);
        appData->descriptorPool = VK_NULL_HANDLE;
        
        vkDestroyBuffer(appData->device, appData->projMatrixBuffer, nullptr);
        appData->projMatrixBuffer = VK_NULL_HANDLE;
        
        vkDestroyBuffer(appData->device, appData->viewMatrixBuffer, nullptr);
        appData->viewMatrixBuffer = VK_NULL_HANDLE;
        
        vkDestroyBuffer(appData->device, appData->modelMatrixBuffer, nullptr);
        appData->modelMatrixBuffer = VK_NULL_HANDLE;
        
        vkFreeMemory(appData->device, appData->matrixBuffersDeviceMemory, nullptr);
        appData->matrixBuffersDeviceMemory = VK_NULL_HANDLE;
        
        vkDestroyBuffer(appData->device, appData->patchBuffer, nullptr);
        appData->patchBuffer = VK_NULL_HANDLE;
        
        vkFreeMemory(appData->device, appData->patchBufferDeviceMemory, nullptr);
        appData->patchBufferDeviceMemory = VK_NULL_HANDLE;
        
        vkDestroyBuffer(appData->device, appData->indexBuffer, nullptr);
        appData->indexBuffer = VK_NULL_HANDLE;
        
        vkFreeMemory(appData->device, appData->indexBufferDeviceMemory, nullptr);
        appData->indexBufferDeviceMemory = VK_NULL_HANDLE;
        
        vkDestroyBuffer(appData->device, appData->vertexBuffer, nullptr);
        appData->vertexBuffer = VK_NULL_HANDLE;
        
        vkFreeMemory(appData->device, appData->vertexBufferDeviceMemory, nullptr);
        appData->vertexBufferDeviceMemory = VK_NULL_HANDLE;
        
        vkDestroyShaderModule(appData->device, appData->vertexShaderModule, nullptr);
        appData->vertexShaderModule = VK_NULL_HANDLE;
        
        vkDestroyShaderModule(appData->device, appData->tessControlShaderModule, nullptr);
        appData->tessControlShaderModule = VK_NULL_HANDLE;
        
        vkDestroyShaderModule(appData->device, appData->tessEvaluationShaderModule, nullptr);
        appData->tessEvaluationShaderModule = VK_NULL_HANDLE;
        
        vkDestroyShaderModule(appData->device, appData->fragmentShaderModule, nullptr);
        appData->fragmentShaderModule = VK_NULL_HANDLE;
        
        vkDestroyDevice(appData->device, nullptr);
        appData->device = VK_NULL_HANDLE;
    }
    
    if (appData->instance)
    {
        vkDestroySurfaceKHR(appData->instance, appData->surface, nullptr);
        appData->surface = VK_NULL_HANDLE;
    }
    
#ifdef ENABLE_VULKAN_DEBUG_UTILS
    PFN_vkDestroyDebugUtilsMessengerEXT const func{
            reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(appData->instance,
                                                                                        "vkDestroyDebugUtilsMessengerEXT"))};
    
    if (func)
    {
        func(appData->instance, appData->debugUtilsMessenger, nullptr);
        appData->debugUtilsMessenger = VK_NULL_HANDLE;
    }
#endif
    
    vkDestroyInstance(appData->instance, nullptr);
    appData->instance = VK_NULL_HANDLE;
    
    glfwTerminate();
    
    return std::move(appData);
}