#include "utils/error_message.hpp"
#include "create_local_device_buffer.hpp"
#include "get_supported_memory_property_index.hpp"
#include "set_debug_utils_object_name.hpp"

#include <cassert>
#include <cstring>
#include <stdexcept>

namespace
{

LocalDeviceBufferDataPtr create_staging_buffer(LocalDeviceBufferDataPtr bufferData)
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
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to create staging buffer"), std::move(*bufferData.release())};
    
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(bufferData->stagingBuffer),
                                "create_local_device_buffer: staging buffer");
    
    return bufferData;
}

LocalDeviceBufferDataPtr find_staging_buffer_memory_type(LocalDeviceBufferDataPtr bufferData)
{
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(bufferData->device, bufferData->stagingBuffer, &memRequirements);
    
    try
    {
        auto const memPropIndex{get_supported_memory_property_index(bufferData->physicalDevice,
                                                                    memRequirements.memoryTypeBits,
                                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)};
    
        bufferData->stagingBufferMemoryTypeIndex = memPropIndex;
    }
    catch (std::runtime_error const & /*error*/)
    {
        std::throw_with_nested(LocalDeviceBufferDataError{ERROR_MESSAGE("failed to find memory index for staging buffer"), std::move(*bufferData.release())});
    }
    
    return bufferData;
}
    
    LocalDeviceBufferDataPtr create_staging_device_memory(LocalDeviceBufferDataPtr bufferData)
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
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to allocate staging buffer memory"), std::move(*bufferData.release())};
    
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(bufferData->stagingBufferDeviceMemory),
                                "create_local_device_buffer: staging buffer device memory");
    
    return bufferData;
}

LocalDeviceBufferDataPtr bind_staging_buffer_and_memory(LocalDeviceBufferDataPtr bufferData)
{
    if (vkBindBufferMemory(bufferData->device, bufferData->stagingBuffer, bufferData->stagingBufferDeviceMemory, 0) != VK_SUCCESS)
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to bind staging buffer and memory"), std::move(*bufferData.release())};
    
    return bufferData;
}

LocalDeviceBufferDataPtr copy_data_to_staging_buffer(LocalDeviceBufferDataPtr bufferData)
{
    void * mappedDataPtr{nullptr};
    if (vkMapMemory(bufferData->device, bufferData->stagingBufferDeviceMemory, 0, bufferData->dataSize, 0, &mappedDataPtr) != VK_SUCCESS)
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to map staging buffer memory"), std::move(*bufferData.release())};
    
    std::memcpy(mappedDataPtr, bufferData->data, static_cast<size_t>(bufferData->dataSize));
    
    vkUnmapMemory(bufferData->device, bufferData->stagingBufferDeviceMemory);
    
    return bufferData;
}

LocalDeviceBufferDataPtr create_buffer(LocalDeviceBufferDataPtr bufferData)
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
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to create buffer"), std::move(*bufferData.release())};
    
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(bufferData->buffer),
                                "create_local_device_buffer: buffer");
    
    return bufferData;
}

LocalDeviceBufferDataPtr find_buffer_memory_type(LocalDeviceBufferDataPtr bufferData)
{
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(bufferData->device, bufferData->buffer, &memRequirements);
    
    try
    {
        auto const memPropIndex{get_supported_memory_property_index(bufferData->physicalDevice,
                                                                    memRequirements.memoryTypeBits,
                                                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};
    
        bufferData->bufferMemoryTypeIndex = memPropIndex;
    }
    catch(std::runtime_error const & /*error*/)
    {
        std::throw_with_nested(LocalDeviceBufferDataError{ERROR_MESSAGE("failed to find memory index for buffer"), std::move(*bufferData.release())});
    }
    
    return bufferData;
}

LocalDeviceBufferDataPtr create_device_memory(LocalDeviceBufferDataPtr bufferData)
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
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to allocate buffer memory"), std::move(*bufferData.release())};
    
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(bufferData->bufferDeviceMemory),
                                "create_local_device_buffer: buffer device memory");
    
    return bufferData;
}

LocalDeviceBufferDataPtr bind_buffer_and_memory(LocalDeviceBufferDataPtr bufferData)
{
    if (vkBindBufferMemory(bufferData->device, bufferData->buffer, bufferData->bufferDeviceMemory, 0) != VK_SUCCESS)
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to bind buffer and memory"), std::move(*bufferData.release())};
    
    return bufferData;
}

LocalDeviceBufferDataPtr create_copy_command_pool(LocalDeviceBufferDataPtr bufferData)
{
    assert(!bufferData->commandPool);
    
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.queueFamilyIndex = bufferData->copyQueueFamilyIndex;
    
    if (vkCreateCommandPool(bufferData->device, &info, nullptr, &bufferData->commandPool) != VK_SUCCESS)
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to create command pool"), std::move(*bufferData.release())};
    
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_COMMAND_POOL,
                                reinterpret_cast<uint64_t>(bufferData->commandPool),
                                "create_local_device_buffer: command pool");
    
    return bufferData;
}

LocalDeviceBufferDataPtr allocate_command_buffer(LocalDeviceBufferDataPtr bufferData)
{
    assert(!bufferData->commandBuffer);
    
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = bufferData->commandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;
    
    if (vkAllocateCommandBuffers(bufferData->device, &info, &bufferData->commandBuffer) != VK_SUCCESS)
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to allocate command buffer"), std::move(*bufferData.release())};
    
    set_debug_utils_object_name(bufferData->instance,
                                bufferData->device,
                                VK_OBJECT_TYPE_COMMAND_BUFFER,
                                reinterpret_cast<uint64_t>(bufferData->commandBuffer),
                                "create_local_device_buffer: command buffer");
    
    return bufferData;
}

LocalDeviceBufferDataPtr copy_buffer(LocalDeviceBufferDataPtr bufferData)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    
    if (vkBeginCommandBuffer(bufferData->commandBuffer, &beginInfo) != VK_SUCCESS)
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to begin command buffer"), std::move(*bufferData.release())};
    
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
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to end command buffer"), std::move(*bufferData.release())};
    
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
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to submit queue"), std::move(*bufferData.release())};
    
    if (vkQueueWaitIdle(bufferData->copyQueue) != VK_SUCCESS)
        throw LocalDeviceBufferDataError{ERROR_MESSAGE("failed to wait queue idle"), std::move(*bufferData.release())};
    
    return bufferData;
}

LocalDeviceBufferData clean(LocalDeviceBufferData && bufferData) noexcept
{
    vkDestroyBuffer(bufferData.device, bufferData.stagingBuffer, nullptr);
    bufferData.stagingBuffer = VK_NULL_HANDLE;
    
    vkFreeMemory(bufferData.device, bufferData.stagingBufferDeviceMemory, nullptr);
    bufferData.stagingBufferDeviceMemory = VK_NULL_HANDLE;
    
    vkDestroyCommandPool(bufferData.device, bufferData.commandPool, nullptr);
    bufferData.commandPool = VK_NULL_HANDLE;
    
    return bufferData;
}

LocalDeviceBufferDataPtr clean(LocalDeviceBufferDataPtr bufferData) noexcept
{
    auto bufferDataToClean{*bufferData};
    bufferDataToClean = clean(std::move(bufferDataToClean));
    
    *bufferData = bufferDataToClean;
    
    return bufferData;
}

}

LocalDeviceBufferDataPtr create_local_device_buffer(LocalDeviceBufferDataPtr bufferData)
{
    try
    {
        bufferData = create_staging_buffer(std::move(bufferData));
        bufferData = find_staging_buffer_memory_type(std::move(bufferData));
        bufferData = create_staging_device_memory(std::move(bufferData));
        bufferData = bind_staging_buffer_and_memory(std::move(bufferData));
        bufferData = copy_data_to_staging_buffer(std::move(bufferData));
        bufferData = create_buffer(std::move(bufferData));
        bufferData = find_buffer_memory_type(std::move(bufferData));
        bufferData = create_device_memory(std::move(bufferData));
        bufferData = bind_buffer_and_memory(std::move(bufferData));
        bufferData = create_copy_command_pool(std::move(bufferData));
        bufferData = allocate_command_buffer(std::move(bufferData));
        bufferData = copy_buffer(std::move(bufferData));
    }
    catch (LocalDeviceBufferDataError & error)
    {
        error.bufferData = clean(std::move(error.bufferData));
        std::throw_with_nested(LocalDeviceBufferDataError{ERROR_MESSAGE("failed to create local device buffer"), std::move(error.bufferData)});
    }
    
    bufferData = clean(std::move(bufferData));
    
    return bufferData;
}