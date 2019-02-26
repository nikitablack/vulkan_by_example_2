#include "helpers/get_supported_memory_property_index.h"
#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"
#include "Global.h"

#include <cassert>

namespace {

VkDeviceSize get_next_alignment_up(VkDeviceSize const baseSize, VkDeviceSize const alignment)
{
    return (baseSize + alignment - 1) & (~(alignment - 1));
}

MaybeAppDataPtr create_buffers(AppDataPtr appData) noexcept
{
    VkDeviceSize bufferSize{shaderDataSize * 16};
    bufferSize = get_next_alignment_up(bufferSize,
                                       appData->physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    
    appData->matrixBufferOffset = bufferSize;
    
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.size = bufferSize * numConcurrentResources;
    info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    
    if (vkCreateBuffer(appData->device, &info, nullptr, &appData->projMatrixBuffer) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create projection matrix buffer", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->projMatrixBuffer),
                                "projection matrix buffer");
#endif
    
    if (vkCreateBuffer(appData->device, &info, nullptr, &appData->viewMatrixBuffer) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create view matrix buffer", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->viewMatrixBuffer),
                                "view matrix buffer");
#endif
    
    if (vkCreateBuffer(appData->device, &info, nullptr, &appData->modelMatrixBuffer) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create model matrix buffer", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->modelMatrixBuffer),
                                "model matrix buffer");
#endif
    
    return std::move(appData);
}

MaybeAppDataPtr allocate_memory(AppDataPtr appData) noexcept
{
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(appData->device, appData->projMatrixBuffer, &memRequirements);
    
    auto const mbMemPropIndex{get_supported_memory_property_index(appData->physicalDevice,
                                                                  memRequirements.memoryTypeBits,
                                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)};
    
    if (!mbMemPropIndex)
        return tl::make_unexpected(AppDataError{"failed to find memory index for matrix buffers", std::move(appData)});
    
    VkDeviceSize const alignedBufferSize{get_next_alignment_up(memRequirements.size,
                                                               memRequirements.alignment)};
    
    VkMemoryAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.allocationSize = alignedBufferSize * 3;
    info.memoryTypeIndex = *mbMemPropIndex;
    
    if (vkAllocateMemory(appData->device, &info, nullptr, &appData->matrixBuffersDeviceMemory) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to allocate matrix buffers memory", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(appData->matrixBuffersDeviceMemory),
                                "matrix buffers device memory");
#endif
    
    return std::move(appData);
}

MaybeAppDataPtr bind_buffers(AppDataPtr appData) noexcept
{
    VkMemoryRequirements memRequirements{};
    
    // need to call vkGetBufferMemoryRequirements() for all buffers as well to make layers not to complain on binding a buffer for which vkGetBufferMemoryRequirements() was not called
    vkGetBufferMemoryRequirements(appData->device, appData->projMatrixBuffer, &memRequirements);
    vkGetBufferMemoryRequirements(appData->device, appData->viewMatrixBuffer, &memRequirements);
    vkGetBufferMemoryRequirements(appData->device, appData->modelMatrixBuffer, &memRequirements);
    
    VkDeviceSize const alignedBufferSize{get_next_alignment_up(memRequirements.size,
                                                               memRequirements.alignment)};
    
    VkDeviceSize offset{0};
    
    if (vkBindBufferMemory(appData->device, appData->projMatrixBuffer, appData->matrixBuffersDeviceMemory, offset) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to bind project matrix buffer", std::move(appData)});
    
    offset += alignedBufferSize;
    if (vkBindBufferMemory(appData->device, appData->viewMatrixBuffer, appData->matrixBuffersDeviceMemory, offset) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to bind view matrix buffer", std::move(appData)});
    
    offset += alignedBufferSize;
    if (vkBindBufferMemory(appData->device, appData->modelMatrixBuffer, appData->matrixBuffersDeviceMemory, offset) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to bind view matrix buffer", std::move(appData)});
    
    return std::move(appData);
}

}

MaybeAppDataPtr create_matrix_buffers(AppDataPtr appData) noexcept
{
    assert(!appData->projMatrixBuffer);
    assert(!appData->viewMatrixBuffer);
    assert(!appData->modelMatrixBuffer);
    assert(!appData->matrixBuffersDeviceMemory);
    
    auto mbAppData{create_buffers(std::move(appData))
                   .and_then(allocate_memory)
                   .and_then(bind_buffers)};
    
    return std::move(mbAppData);
}