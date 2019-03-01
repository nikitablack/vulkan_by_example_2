#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <array>
#include <cassert>

MaybeAppDataPtr create_framebuffers(AppDataPtr appData) noexcept
{
    assert(appData->framebuffers.empty());
    
    appData->framebuffers.resize(appData->swapchainImageViews.size());
    
    for(size_t i{0}; i < appData->swapchainImageViews.size(); ++i)
    {
        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.renderPass = appData->renderPass;
        info.attachmentCount = 1;
        info.pAttachments = &appData->swapchainImageViews[i];
        info.width = appData->surfaceExtent.width;
        info.height = appData->surfaceExtent.height;
        info.layers = 1;
        
        if (vkCreateFramebuffer(appData->device, &info, nullptr, &appData->framebuffers[i]) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to create framebuffer", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_FRAMEBUFFER,
                                    reinterpret_cast<uint64_t>(appData->framebuffers[i]),
                                    std::string{"framebuffer " + std::to_string(i)}.c_str());
#endif
    }
    
    return std::move(appData);
}