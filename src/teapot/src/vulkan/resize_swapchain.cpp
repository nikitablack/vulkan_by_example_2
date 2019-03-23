#include "teapot_vulkan.h"

MaybeAppDataPtr resize_swapchain(AppDataPtr appData) noexcept
{
    if (vkDeviceWaitIdle(appData->device) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed device to wait idle on swapchain resize", std::move(appData)});
    
    for (auto const framebuffer : appData->framebuffers)
        vkDestroyFramebuffer(appData->device, framebuffer, nullptr);
    
    appData->framebuffers.clear();
    
    for(auto const imageView : appData->swapchainImageViews)
        vkDestroyImageView(appData->device, imageView, nullptr);
    
    appData->swapchainImageViews.clear();
    
    appData->swapchainImages.clear();

    vkDestroyImage(appData->device, appData->depthImage, nullptr);
    appData->depthImage = VK_NULL_HANDLE;

    vkDestroyImageView(appData->device, appData->depthImageView, nullptr);
    appData->depthImageView = VK_NULL_HANDLE;
    
    vkFreeMemory(appData->device, appData->depthImageMemory, nullptr);
    appData->depthImageMemory = VK_NULL_HANDLE;

    auto mbAppData{create_swap_chain(std::move(appData))
                   .and_then(get_swapchain_images_and_views)
                   .and_then(create_depth_buffer_and_view)
                   .and_then(create_framebuffers)};
    
    if (!mbAppData)
        return mbAppData;
    
    appData = std::move(*mbAppData);
    
    return std::move(appData);
}