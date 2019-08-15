#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <array>
#include <cassert>

AppDataPtr create_framebuffers(AppDataPtr appData)
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
            throw AppDataError{ERROR_MESSAGE("failed to create framebuffer"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_FRAMEBUFFER,
                                    reinterpret_cast<uint64_t>(appData->framebuffers[i]),
                                    std::string{"framebuffer " + std::to_string(i)}.c_str());
    }
    
    return appData;
}