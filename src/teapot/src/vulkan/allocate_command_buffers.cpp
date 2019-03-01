#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"
#include "Global.h"

#include <cassert>

MaybeAppDataPtr allocate_command_buffers(AppDataPtr appData) noexcept
{
    assert(appData->commandBuffers.empty());
    
    appData->commandBuffers.resize(numConcurrentResources);
    
    for (uint32_t i{0}; i < numConcurrentResources; ++i)
    {
        VkCommandBufferAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.commandPool = appData->commandPool;
        info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        
        if (vkAllocateCommandBuffers(appData->device, &info, &appData->commandBuffers[i]) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to allocate command buffer", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_COMMAND_BUFFER,
                                    reinterpret_cast<uint64_t>(appData->commandBuffers[i]),
                                    std::string{"command buffer " + std::to_string(i)}.c_str());
#endif
    }
    
    return std::move(appData);
}