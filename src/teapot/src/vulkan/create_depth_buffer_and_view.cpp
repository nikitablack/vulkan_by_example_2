#include "helpers/get_supported_memory_property_index.h"
#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <array>
#include <cassert>

namespace
{

MaybeAppDataPtr create_image(AppDataPtr appData) noexcept
{
    assert(!appData->depthImage);
    
    VkImageCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = appData->depthFormat;
    info.extent = VkExtent3D{appData->surfaceExtent.width, appData->surfaceExtent.height, 1};
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    info.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    if (vkCreateImage(appData->device, &info, nullptr, &appData->depthImage) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create depth image", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_IMAGE,
                                reinterpret_cast<uint64_t>(appData->depthImage),
                                "depth image");
#endif
    
    return std::move(appData);
}

MaybeAppDataPtr allocate_device_memory(AppDataPtr appData) noexcept
{
    assert(!appData->depthImageMemory);
    
    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(appData->device, appData->depthImage, &memRequirements);
    
    auto const mbMemPropIndex{get_supported_memory_property_index(appData->physicalDevice,
                                                                  memRequirements.memoryTypeBits,
                                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};
    
    if (!mbMemPropIndex)
        return tl::make_unexpected(AppDataError{"failed to find memory index for depth image", std::move(appData)});
    
    VkMemoryAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.allocationSize = memRequirements.size;
    info.memoryTypeIndex = *mbMemPropIndex;
    
    if (vkAllocateMemory(appData->device, &info, nullptr, &appData->depthImageMemory) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to allocate depth image memory", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(appData->depthImageMemory),
                                "depth image device memory");
#endif
    return std::move(appData);
}

MaybeAppDataPtr bind_image_and_memory(AppDataPtr appData) noexcept
{
    if (vkBindImageMemory(appData->device, appData->depthImage, appData->depthImageMemory, 0) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to bind depth image", std::move(appData)});
    
    return std::move(appData);
}

MaybeAppDataPtr create_image_view(AppDataPtr appData) noexcept
{
    assert(!appData->depthImageView);
    
    VkImageViewCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.image = appData->depthImage;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = appData->depthFormat;
    info.components = VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    info.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    
    if (vkCreateImageView(appData->device, &info, nullptr, &appData->depthImageView) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create depth image view", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_IMAGE_VIEW,
                                reinterpret_cast<uint64_t>(appData->depthImageView),
                                "depth image view ");
#endif
    
    return std::move(appData);
}

} // namespace

MaybeAppDataPtr create_depth_buffer_and_view(AppDataPtr appData) noexcept
{
    auto mbAppData{create_image(std::move(appData))
                   .and_then(allocate_device_memory)
                   .and_then(bind_image_and_memory)
                   .and_then(create_image_view)};
    
    return mbAppData;
}