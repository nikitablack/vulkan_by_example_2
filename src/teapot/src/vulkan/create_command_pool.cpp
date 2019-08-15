#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>

AppDataPtr create_command_pool(AppDataPtr appData)
{
    assert(!appData->graphicsCommandPool);
    
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = appData->graphicsFamilyQueueIndex;
    
    if (vkCreateCommandPool(appData->device, &info, nullptr, &appData->graphicsCommandPool) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create graphics command pool"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_COMMAND_POOL,
                                reinterpret_cast<uint64_t>(appData->graphicsCommandPool),
                                "graphics command pool");
    
    if(appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        info.queueFamilyIndex = appData->presentFamilyQueueIndex;
    
        if (vkCreateCommandPool(appData->device, &info, nullptr, &appData->presentCommandPool) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create command pool"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_COMMAND_POOL,
                                    reinterpret_cast<uint64_t>(appData->presentCommandPool),
                                    "present command pool");
    }
    
    return appData;
}