#include "utils/error_message.hpp"
#include "helpers/get_supported_memory_property_index.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <array>
#include <cassert>

namespace
{

AppDataPtr create_image(AppDataPtr appData)
{
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
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    
    if (vkCreateImage(appData->device, &info, nullptr, &appData->depthImage) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create depth image"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_IMAGE,
                                reinterpret_cast<uint64_t>(appData->depthImage),
                                "depth image");
    
    return appData;
}

AppDataPtr allocate_device_memory(AppDataPtr appData)
{
    VkMemoryRequirements memRequirements{};
    vkGetImageMemoryRequirements(appData->device, appData->depthImage, &memRequirements);
    
    VkMemoryAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.allocationSize = memRequirements.size;
    
    try
    {
        info.memoryTypeIndex = get_supported_memory_property_index(appData->physicalDevice,
                                                                   memRequirements.memoryTypeBits,
                                                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }
    catch(std::runtime_error const & error)
    {
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to allocate device memory"), std::move(*appData.release())});
    }
    
    if (vkAllocateMemory(appData->device, &info, nullptr, &appData->depthImageMemory) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to allocate depth image memory"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(appData->depthImageMemory),
                                "depth image device memory");
    
    return appData;
}

AppDataPtr bind_image_and_memory(AppDataPtr appData)
{
    if (vkBindImageMemory(appData->device, appData->depthImage, appData->depthImageMemory, 0) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to bind depth image"), std::move(*appData.release())};
    
    return appData;
}

AppDataPtr create_image_view(AppDataPtr appData)
{
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
        throw AppDataError{ERROR_MESSAGE("failed to create depth image view"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_IMAGE_VIEW,
                                reinterpret_cast<uint64_t>(appData->depthImageView),
                                "depth image view");
    
    return appData;
}

} // namespace

AppDataPtr create_depth_buffer_and_view(AppDataPtr appData)
{
    assert(!appData->depthImage);
    assert(!appData->depthImageMemory);
    assert(!appData->depthImageView);
    
    try
    {
        appData = create_image(std::move(appData));
        appData = allocate_device_memory(std::move(appData));
        appData = bind_image_and_memory(std::move(appData));
        appData = create_image_view(std::move(appData));
    }
    catch(AppDataError & error)
    {
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to create depth buffer and view"), std::move(error.appData)});
    }
    
    return appData;
}