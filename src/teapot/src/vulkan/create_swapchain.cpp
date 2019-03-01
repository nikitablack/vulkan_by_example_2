#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <algorithm>
#include <cassert>

MaybeAppDataPtr create_swap_chain(AppDataPtr appData) noexcept
{
    auto const oldSwapChain{appData->swapchain};
    
    VkSurfaceCapabilitiesKHR surfaceCapabilities{};
    
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(appData->physicalDevice, appData->surface, &surfaceCapabilities) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to fet physical device surface capabilities", std::move(appData)});
    
    int windowWidth{0};
    int windowHeight{0};
    glfwGetFramebufferSize(appData->window, &windowWidth, &windowHeight);
    
    if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
    {
        appData->surfaceExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, static_cast<uint32_t>(windowWidth)));
        appData->surfaceExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, static_cast<uint32_t>(windowHeight)));
    }
    else
        appData->surfaceExtent = surfaceCapabilities.currentExtent;
    
    uint32_t const imageCount{std::min(std::max(surfaceCapabilities.minImageCount, uint32_t{3}),
                                       surfaceCapabilities.maxImageCount)};
    
    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.flags = 0;
    info.surface = appData->surface;
    info.minImageCount = imageCount;
    info.imageFormat = appData->surfaceFormat.format;
    info.imageColorSpace = appData->surfaceFormat.colorSpace;
    info.imageExtent = appData->surfaceExtent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = nullptr;
    info.preTransform = surfaceCapabilities.currentTransform;
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
    
    if(oldSwapChain)
    {
        auto const destroySwapChainFunc{reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetInstanceProcAddr(appData->instance,
                                                                                                          "vkDestroySwapchainKHR"))};
        
        if (!destroySwapChainFunc)
            return tl::make_unexpected(AppDataError{"failed to destroy old swapchain", std::move(appData)});
        
        destroySwapChainFunc(appData->device, oldSwapChain, nullptr);
    }
    
    return std::move(appData);
}