#include "utils/error_message.hpp"
#include "vulkan/helpers/get_supported_memory_property_index.hpp"
#include "vulkan/helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"
#include "Global.hpp"

#include <cassert>

namespace
{

VkDeviceSize get_next_alignment_up(VkDeviceSize const baseSize, VkDeviceSize const alignment)
{
    return (baseSize + alignment - 1) & (~(alignment - 1));
}

AppDataPtr create_buffers(AppDataPtr appData)
{
    VkDeviceSize dataSize{shaderDataSize * 16};
    dataSize = get_next_alignment_up(dataSize,
                                     appData->physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    
    appData->matrixBufferOffset = dataSize;
    
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.size = dataSize * numConcurrentResources;
    info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    
    if (vkCreateBuffer(appData->device, &info, nullptr, &appData->projMatrixBuffer) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create projection matrix buffer"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->projMatrixBuffer),
                                "projection matrix buffer");
    
    if (vkCreateBuffer(appData->device, &info, nullptr, &appData->viewMatrixBuffer) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create view matrix buffer"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->viewMatrixBuffer),
                                "view matrix buffer");
    
    if (vkCreateBuffer(appData->device, &info, nullptr, &appData->modelMatrixBuffer) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create model matrix buffer"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->modelMatrixBuffer),
                                "model matrix buffer");
    
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(appData->device, appData->projMatrixBuffer, &memRequirements);
    
    appData->matrixBufferSizeAligned = get_next_alignment_up(memRequirements.size,
                                                             memRequirements.alignment);

    return appData;
}

AppDataPtr allocate_memory(AppDataPtr appData)
{
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(appData->device, appData->projMatrixBuffer, &memRequirements);
    
    uint32_t memPropIndex;
    
    try
    {
        memPropIndex = get_supported_memory_property_index(appData->physicalDevice,
                                                           memRequirements.memoryTypeBits,
                                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    }
    catch (std::runtime_error const & error)
    {
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to find memory index for matrix buffers"), std::move(*appData.release())});
    }
    
    VkMemoryAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.allocationSize = appData->matrixBufferSizeAligned * 3;
    info.memoryTypeIndex = memPropIndex;
    
    if (vkAllocateMemory(appData->device, &info, nullptr, &appData->matrixBuffersDeviceMemory) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to allocate matrix buffers memory"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(appData->matrixBuffersDeviceMemory),
                                "matrix buffers device memory");
    
    return appData;
}

AppDataPtr bind_buffers(AppDataPtr appData)
{
    VkMemoryRequirements memRequirements{};
    
    // need to call vkGetBufferMemoryRequirements() for all buffers as well to make layers not to complain on binding a buffer for which vkGetBufferMemoryRequirements() was not called
    vkGetBufferMemoryRequirements(appData->device, appData->projMatrixBuffer, &memRequirements);
    vkGetBufferMemoryRequirements(appData->device, appData->viewMatrixBuffer, &memRequirements);
    vkGetBufferMemoryRequirements(appData->device, appData->modelMatrixBuffer, &memRequirements);
    
    if (vkBindBufferMemory(appData->device, appData->projMatrixBuffer, appData->matrixBuffersDeviceMemory, 0) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to bind project matrix buffer"), std::move(*appData.release())};
    
    if (vkBindBufferMemory(appData->device, appData->viewMatrixBuffer, appData->matrixBuffersDeviceMemory, appData->matrixBufferSizeAligned) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to bind view matrix buffer"), std::move(*appData.release())};
    
    if (vkBindBufferMemory(appData->device, appData->modelMatrixBuffer, appData->matrixBuffersDeviceMemory, appData->matrixBufferSizeAligned * 2) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to bind model matrix buffer"), std::move(*appData.release())};
    
    return appData;
}

}

AppDataPtr create_matrix_buffers(AppDataPtr appData)
{
    assert(!appData->projMatrixBuffer);
    assert(!appData->viewMatrixBuffer);
    assert(!appData->modelMatrixBuffer);
    assert(!appData->matrixBuffersDeviceMemory);
    
    try
    {
        appData = create_buffers(std::move(appData));
        appData = allocate_memory(std::move(appData));
        appData = bind_buffers(std::move(appData));
    }
    catch(AppDataError & error)
    {
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to create matrix buffers"), std::move(error.appData)});
    }
    
    return appData;
}