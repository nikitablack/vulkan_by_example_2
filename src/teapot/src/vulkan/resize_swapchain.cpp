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
    
    auto mbAppData{create_swap_chain(std::move(appData))
                   .and_then(get_swapchain_images_and_views)
                   .and_then(create_framebuffers)};
    
    if (!mbAppData)
        std::move(mbAppData);
    
    appData = std::move(*mbAppData);
    
    return std::move(appData);
}