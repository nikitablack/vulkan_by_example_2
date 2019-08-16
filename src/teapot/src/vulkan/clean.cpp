#include "teapot_vulkan.hpp"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

void clean(AppData && appData) noexcept
{
    if (appData.device)
    {
        if(vkDeviceWaitIdle(appData.device) != VK_SUCCESS)
            return;
    
        vkDestroyImage(appData.device, appData.depthImage, nullptr);
        vkDestroyImageView(appData.device, appData.depthImageView, nullptr);
        vkFreeMemory(appData.device, appData.depthImageMemory, nullptr);
        
        for (auto const fence : appData.graphicsFences)
            vkDestroyFence(appData.device, fence, nullptr);
    
        for (auto const fence : appData.presentFences)
            vkDestroyFence(appData.device, fence, nullptr);
        
        vkDestroyCommandPool(appData.device, appData.graphicsCommandPool, nullptr);
        vkDestroyCommandPool(appData.device, appData.presentCommandPool, nullptr);
        vkDestroySemaphore(appData.device, appData.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(appData.device, appData.graphicsFinishedSemaphore, nullptr);
        vkDestroySemaphore(appData.device, appData.queueOwnershipChangedSemaphore, nullptr);
        
        for (auto const framebuffer : appData.framebuffers)
            vkDestroyFramebuffer(appData.device, framebuffer, nullptr);
        
        for (auto const imageView : appData.swapchainImageViews)
            vkDestroyImageView(appData.device, imageView, nullptr);
    
        auto const destroySwapChainFunc{ reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetInstanceProcAddr(appData.instance, "vkDestroySwapchainKHR")) };
        if (destroySwapChainFunc)
        {
            destroySwapChainFunc(appData.device, appData.swapchain, nullptr);
        }
        
        vkDestroyRenderPass(appData.device, appData.renderPass, nullptr);
        vkDestroyPipelineLayout(appData.device, appData.pipelineLayout, nullptr);
        vkDestroyPipeline(appData.device, appData.solidPipeline, nullptr);
        vkDestroyPipeline(appData.device, appData.wireframePipeline, nullptr);
        
        vkDestroyDescriptorSetLayout(appData.device, appData.descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(appData.device, appData.descriptorPool, nullptr);
        
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