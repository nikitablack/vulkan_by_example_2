#include "utils/error_message.hpp"
#include "teapot_vulkan.hpp"

AppDataPtr resize_swapchain(AppDataPtr appData)
{
    if (vkDeviceWaitIdle(appData->device) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed device to wait device to complete"), std::move(*appData.release())};
    
    for (auto const framebuffer : appData->framebuffers)
        vkDestroyFramebuffer(appData->device, framebuffer, nullptr);
    
    appData->framebuffers.clear();
    
    for(auto const imageView : appData->swapchainImageViews)
        vkDestroyImageView(appData->device, imageView, nullptr);
    
    appData->swapchainImageViews.clear();
    
    appData->swapchainImages.clear();
    
    try
    {
        appData = create_swapchain(std::move(appData));
        appData = get_swapchain_images_and_views(std::move(appData));
        appData = create_framebuffers(std::move(appData));
    }
    catch(AppDataError & error)
    {
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to resize swapchain"), std::move(error.appData)});
    }
    
    return appData;
}