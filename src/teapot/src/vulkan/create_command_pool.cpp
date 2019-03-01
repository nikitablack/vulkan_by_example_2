#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <cassert>

MaybeAppDataPtr create_command_pool(AppDataPtr appData) noexcept
{
    assert(!appData->commandPool);
    
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = appData->graphicsFamilyQueueIndex;
    
    if (vkCreateCommandPool(appData->device, &info, nullptr, &appData->commandPool) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create command pool", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_COMMAND_POOL,
                                reinterpret_cast<uint64_t>(appData->commandPool),
                                "command pool");
#endif
    
    return std::move(appData);
}