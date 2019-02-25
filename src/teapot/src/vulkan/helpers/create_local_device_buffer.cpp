#include "create_local_device_buffer.h"
#include "get_supported_memory_property_index.h"
#include "set_debug_utils_object_name.h"

#include <cassert>
#include <cstring>

namespace {

MaybeLocalDeviceBufferDataPtr create_staging_buffer(LocalDeviceBufferDataPtr bufferData) noexcept
{
    assert(!bufferData->stagingBuffer);
    
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.size = bufferData->dataSize;
    info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    
    if (vkCreateBuffer(bufferData->device, &info, nullptr, &bufferData->stagingBuffer) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to create staging buffer", std::move(bufferData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(bufferData->stagingBuffer),
                                "create_local_device_buffer: staging buffer");
#endif
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr find_staging_buffer_memory_type(LocalDeviceBufferDataPtr bufferData) noexcept
{
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(bufferData->device, bufferData->stagingBuffer, &memRequirements);
    
    auto const mbMemPropIndex{get_supported_memory_property_index(bufferData->physicalDevice,
                                                                  memRequirements.memoryTypeBits,
                                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)};
    
    if (!mbMemPropIndex)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to find memory index for staging buffer", std::move(bufferData)});
    
    bufferData->stagingBufferMemoryTypeIndex = *mbMemPropIndex;
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr create_staging_device_memory(LocalDeviceBufferDataPtr bufferData) noexcept
{
    assert(!bufferData->stagingBufferDeviceMemory);
    
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(bufferData->device, bufferData->stagingBuffer, &memRequirements);
    
    VkMemoryAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.allocationSize = memRequirements.size;
    info.memoryTypeIndex = bufferData->stagingBufferMemoryTypeIndex;
    
    if (vkAllocateMemory(bufferData->device, &info, nullptr, &bufferData->stagingBufferDeviceMemory) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to allocate staging buffer memory", std::move(bufferData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(bufferData->stagingBufferDeviceMemory),
                                "create_local_device_buffer: staging buffer device memory");
#endif
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr bind_staging_buffer_and_memory(LocalDeviceBufferDataPtr bufferData) noexcept
{
    if (vkBindBufferMemory(bufferData->device, bufferData->stagingBuffer, bufferData->stagingBufferDeviceMemory, 0) !=
        VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to bind staging buffer and memory", std::move(bufferData)});
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr copy_data_to_staging_buffer(LocalDeviceBufferDataPtr bufferData) noexcept
{
    void * mappedDataPtr{nullptr};
    if (vkMapMemory(bufferData->device, bufferData->stagingBufferDeviceMemory, 0, bufferData->dataSize, 0,
                    &mappedDataPtr) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to map staging buffer memory", std::move(bufferData)});
    
    std::memcpy(mappedDataPtr, bufferData->data, static_cast<size_t>(bufferData->dataSize));
    
    vkUnmapMemory(bufferData->device, bufferData->stagingBufferDeviceMemory);
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr create_buffer(LocalDeviceBufferDataPtr bufferData) noexcept
{
    assert(!bufferData->buffer);
    
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.size = bufferData->dataSize;
    info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferData->localDeviceBufferUsage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    
    if (vkCreateBuffer(bufferData->device, &info, nullptr, &bufferData->buffer) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to create buffer", std::move(bufferData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(bufferData->buffer),
                                "create_local_device_buffer: buffer");
#endif
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr find_buffer_memory_type(LocalDeviceBufferDataPtr bufferData) noexcept
{
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(bufferData->device, bufferData->buffer, &memRequirements);
    
    auto const mbMemPropIndex{get_supported_memory_property_index(bufferData->physicalDevice,
                                                                  memRequirements.memoryTypeBits,
                                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};
    
    if (!mbMemPropIndex)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to find memory index for buffer", std::move(bufferData)});
    
    bufferData->bufferMemoryTypeIndex = *mbMemPropIndex;
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr create_device_memory(LocalDeviceBufferDataPtr bufferData) noexcept
{
    assert(!bufferData->bufferDeviceMemory);
    
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(bufferData->device, bufferData->buffer, &memRequirements);
    
    VkMemoryAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.allocationSize = memRequirements.size;
    info.memoryTypeIndex = bufferData->bufferMemoryTypeIndex;
    
    if (vkAllocateMemory(bufferData->device, &info, nullptr, &bufferData->bufferDeviceMemory) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to allocate buffer memory", std::move(bufferData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(bufferData->bufferDeviceMemory),
                                "create_local_device_buffer: buffer device memory");
#endif
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr bind_buffer_and_memory(LocalDeviceBufferDataPtr bufferData) noexcept
{
    if (vkBindBufferMemory(bufferData->device, bufferData->buffer, bufferData->bufferDeviceMemory, 0) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to bind buffer and memory", std::move(bufferData)});
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr create_copy_command_pool(LocalDeviceBufferDataPtr bufferData) noexcept
{
    assert(!bufferData->commandPool);
    
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.queueFamilyIndex = bufferData->copyQueueFamilyIndex;
    
    if (vkCreateCommandPool(bufferData->device, &info, nullptr, &bufferData->commandPool) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to create command pool", std::move(bufferData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_COMMAND_POOL,
                                reinterpret_cast<uint64_t>(bufferData->commandPool),
                                "create_local_device_buffer: command pool");
#endif
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr allocate_command_buffer(LocalDeviceBufferDataPtr bufferData) noexcept
{
    assert(!bufferData->commandBuffer);
    
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = bufferData->commandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;
    
    if (vkAllocateCommandBuffers(bufferData->device, &info, &bufferData->commandBuffer) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to allocate command buffer", std::move(bufferData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_COMMAND_BUFFER,
                                reinterpret_cast<uint64_t>(bufferData->commandBuffer),
                                "create_local_device_buffer: command buffer");
#endif
    
    return std::move(bufferData);
}

MaybeLocalDeviceBufferDataPtr copy_buffer(LocalDeviceBufferDataPtr bufferData) noexcept
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    
    if (vkBeginCommandBuffer(bufferData->commandBuffer, &beginInfo) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to begin command buffer", std::move(bufferData)});
    
    {
        VkBufferCopy copy{};
        copy.srcOffset = 0;
        copy.dstOffset = 0;
        copy.size = bufferData->dataSize;
        
        vkCmdCopyBuffer(bufferData->commandBuffer, bufferData->stagingBuffer, bufferData->buffer, 1, &copy);
    }
    
    {
        VkBufferMemoryBarrier afterCopyBarrier{};
        afterCopyBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        afterCopyBarrier.pNext = nullptr;
        afterCopyBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        afterCopyBarrier.dstAccessMask = bufferData->localDeviceBufferAccessMask;
        afterCopyBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        afterCopyBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        afterCopyBarrier.buffer = bufferData->buffer;
        afterCopyBarrier.offset = 0;
        afterCopyBarrier.size = bufferData->dataSize;
        
        vkCmdPipelineBarrier(bufferData->commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             bufferData->localDeviceBufferStageMask, 0, 0, nullptr, 1, &afterCopyBarrier, 0, nullptr);
    }
    
    if (vkEndCommandBuffer(bufferData->commandBuffer) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to end command buffer", std::move(bufferData)});
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &bufferData->commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    
    if (vkQueueSubmit(bufferData->copyQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to submit queue", std::move(bufferData)});
    
    if (vkQueueWaitIdle(bufferData->copyQueue) != VK_SUCCESS)
        return tl::make_unexpected(LocalDeviceBufferDataError{"create_local_device_buffer: failed to wait queue idle", std::move(bufferData)});
    
    return std::move(bufferData);
}

}

MaybeLocalDeviceBufferDataPtr create_local_device_buffer(LocalDeviceBufferDataPtr bufferData) noexcept
{
    auto mbBufferData{create_staging_buffer(std::move(bufferData))
                      .and_then(find_staging_buffer_memory_type)
                      .and_then(create_staging_device_memory)
                      .and_then(bind_staging_buffer_and_memory)
                      .and_then(copy_data_to_staging_buffer)
                      .and_then(create_buffer)
                      .and_then(find_buffer_memory_type)
                      .and_then(create_device_memory)
                      .and_then(bind_buffer_and_memory)
                      .and_then(create_copy_command_pool)
                      .and_then(allocate_command_buffer)
                      .and_then(copy_buffer)};
    
    LocalDeviceBufferData const * rawBufferDataPtr{nullptr};
    
    if(!mbBufferData)
    {
        rawBufferDataPtr = mbBufferData.error().bufferData.get();
    }
    else
    {
        rawBufferDataPtr = (*mbBufferData).get();
    }
    
    vkDestroyBuffer(rawBufferDataPtr->device, rawBufferDataPtr->stagingBuffer, nullptr);
    vkFreeMemory(rawBufferDataPtr->device, rawBufferDataPtr->stagingBufferDeviceMemory, nullptr);
    vkDestroyCommandPool(rawBufferDataPtr->device, rawBufferDataPtr->commandPool, nullptr);
    
    return std::move(mbBufferData);
}