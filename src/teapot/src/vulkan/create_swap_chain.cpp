#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <algorithm>
#include <cassert>

MaybeAppDataPtr create_swap_chain(AppDataPtr appData) noexcept
{
    assert(!appData->swapchain);
    
    uint32_t const imageCount{std::min(std::max(appData->surfaceCapabilities.minImageCount, uint32_t{3}),
                                       appData->surfaceCapabilities.maxImageCount)};
    
    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.flags = 0;
    info.surface = appData->surface;
    info.minImageCount = imageCount;
    info.imageFormat = appData->surfaceFormat.format;
    info.imageColorSpace = appData->surfaceFormat.colorSpace;
    info.imageExtent = /*VkExtent2D{100, 100};*/appData->surfaceExtent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    info.preTransform = appData->surfaceCapabilities.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = appData->surfacePresentMode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = appData->swapchain;
    
    if (vkCreateSwapchainKHR(appData->device, &info, nullptr, &appData->swapchain) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create swap chain", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_SWAPCHAIN_KHR,
                                reinterpret_cast<uint64_t>(appData->swapchain),
                                "swap chain");
#endif
    
    return std::move(appData);
}