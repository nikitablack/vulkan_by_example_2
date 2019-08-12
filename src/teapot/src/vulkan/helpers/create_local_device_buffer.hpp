#pragma once

#include "vulkan/vulkan.h"

#include <memory>
#include <string>

struct LocalDeviceBufferData
{
    VkInstance instance{VK_NULL_HANDLE};
    VkDevice device{VK_NULL_HANDLE};
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VkDeviceSize dataSize{};
    void * data{nullptr};
    VkBufferUsageFlags localDeviceBufferUsage{};
    VkBufferUsageFlags localDeviceBufferAccessMask{};
    VkBufferUsageFlags localDeviceBufferStageMask{};
    uint32_t copyQueueFamilyIndex{};
    VkQueue copyQueue{VK_NULL_HANDLE};
    
    VkBuffer stagingBuffer{};
    uint32_t stagingBufferMemoryTypeIndex{};
    VkDeviceMemory stagingBufferDeviceMemory{};
    
    VkBuffer buffer{};
    uint32_t bufferMemoryTypeIndex{};
    VkDeviceMemory bufferDeviceMemory{};
    
    VkCommandPool commandPool{};
    VkCommandBuffer commandBuffer{VK_NULL_HANDLE};
};

using LocalDeviceBufferDataPtr = std::unique_ptr<LocalDeviceBufferData>;

struct LocalDeviceBufferDataError : public std::exception
{
    LocalDeviceBufferDataError(std::string msg, LocalDeviceBufferData data) : message{std::move(msg)}, bufferData{std::move(data)}
    {}
    
    const char * what() const noexcept override
    {
        return message.c_str();
    }
    
    std::string message{};
    LocalDeviceBufferData bufferData{};
};

LocalDeviceBufferDataPtr create_local_device_buffer(LocalDeviceBufferDataPtr bufferData);