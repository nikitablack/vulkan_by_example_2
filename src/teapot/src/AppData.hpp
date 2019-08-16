#pragma once

#include "TeapotData.hpp"

#include "vulkan/vulkan.h"

#include <exception>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

struct AppData
{
    TeapotData teapotData{};
    std::vector<char const *> layers{};
    std::vector<char const *> instanceExtensions{};
    std::vector<char const *> deviceExtensions{};
    
    GLFWwindow *window{nullptr};
    VkInstance instance{VK_NULL_HANDLE};
    VkSurfaceKHR surface{VK_NULL_HANDLE};
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    uint32_t graphicsFamilyQueueIndex{0};
    uint32_t presentFamilyQueueIndex{0};
    VkSurfaceFormatKHR surfaceFormat{};
    VkPresentModeKHR surfacePresentMode{};
    VkExtent2D surfaceExtent{};
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    VkDevice device{VK_NULL_HANDLE};
    
    VkShaderModule vertexShaderModule{VK_NULL_HANDLE};
    VkShaderModule tessControlShaderModule{VK_NULL_HANDLE};
    VkShaderModule tessEvaluationShaderModule{VK_NULL_HANDLE};
    VkShaderModule fragmentShaderModule{VK_NULL_HANDLE};
    VkQueue graphicsQueue{VK_NULL_HANDLE};
    VkQueue presentQueue{VK_NULL_HANDLE};
    VkBuffer vertexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory vertexBufferDeviceMemory{VK_NULL_HANDLE};
    VkBuffer indexBuffer{VK_NULL_HANDLE};
    VkDeviceMemory indexBufferDeviceMemory{VK_NULL_HANDLE};
    VkBuffer patchBuffer{VK_NULL_HANDLE};
    VkDeviceMemory patchBufferDeviceMemory{VK_NULL_HANDLE};
    VkBuffer projMatrixBuffer{};
    VkBuffer viewMatrixBuffer{};
    VkBuffer modelMatrixBuffer{};
    VkDeviceMemory matrixBuffersDeviceMemory{VK_NULL_HANDLE};
    VkDeviceSize matrixBufferSizeAligned{};
    VkDeviceSize matrixBufferOffset{};
    
    VkDescriptorSetLayout descriptorSetLayout{VK_NULL_HANDLE};
    VkDescriptorPool descriptorPool{VK_NULL_HANDLE};
    std::vector<VkDescriptorSet> descriptorSets{};
    
    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
    VkRenderPass renderPass{VK_NULL_HANDLE};
    VkPipeline wireframePipeline{VK_NULL_HANDLE};
    VkPipeline solidPipeline{VK_NULL_HANDLE};
    
    VkSwapchainKHR swapchain{VK_NULL_HANDLE};
    std::vector<VkImage> swapchainImages{};
    std::vector<VkImageView> swapchainImageViews{};
    std::vector<VkFramebuffer> framebuffers{};
    
    VkSemaphore imageAvailableSemaphore{VK_NULL_HANDLE};
    VkSemaphore graphicsFinishedSemaphore{VK_NULL_HANDLE};
    VkSemaphore queueOwnershipChangedSemaphore{VK_NULL_HANDLE};
    std::vector<VkFence> graphicsFences{};
    std::vector<VkFence> presentFences{};
    VkCommandPool graphicsCommandPool{VK_NULL_HANDLE};
    VkCommandPool presentCommandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> graphicsCommandBuffers{};
    std::vector<VkCommandBuffer> presentCommandBuffers{};
    uint32_t currentResourceIndex{0};
    
    VkFormat depthFormat{};
    VkImage depthImage{VK_NULL_HANDLE};
    VkImageView depthImageView{VK_NULL_HANDLE};
    VkDeviceMemory depthImageMemory{VK_NULL_HANDLE};
};

using AppDataPtr = std::unique_ptr<AppData>;

struct AppDataError : public std::exception
{
    AppDataError(std::string msg, AppData && ptr) : message{std::move(msg)}, appData{std::move(ptr)}
    {}
    
    const char *what() const noexcept override
    {
        return message.c_str();
    }
    
    std::string message{};
    AppData appData{};
};