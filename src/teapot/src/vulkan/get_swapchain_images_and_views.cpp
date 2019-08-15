#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <algorithm>
#include <cassert>

AppDataPtr get_swapchain_images_and_views(AppDataPtr appData)
{
    assert(appData->swapchainImages.empty());
    assert(appData->swapchainImageViews.empty());
    
    uint32_t imageCount{0};
    
    if (vkGetSwapchainImagesKHR(appData->device, appData->swapchain, &imageCount, nullptr) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to get swap chain images"), std::move(*appData.release())};
    
    appData->swapchainImages.resize(imageCount);
    
    if (vkGetSwapchainImagesKHR(appData->device, appData->swapchain, &imageCount, appData->swapchainImages.data()) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to get swap chain images"), std::move(*appData.release())};
    
    appData->swapchainImageViews.resize(appData->swapchainImages.size());
    
    for (size_t i{0}; i < appData->swapchainImages.size(); ++i)
    {
        VkImageViewCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.image = appData->swapchainImages[i];
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = appData->surfaceFormat.format;
        info.components = VkComponentMapping{VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        info.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        
        if (vkCreateImageView(appData->device, &info, nullptr, &appData->swapchainImageViews[i]) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create image view"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_IMAGE_VIEW,
                                    reinterpret_cast<uint64_t>(appData->swapchainImageViews[i]),
                                    std::string{"swapchain image view " + std::to_string(i)}.c_str());
    }
    
    return appData;
}