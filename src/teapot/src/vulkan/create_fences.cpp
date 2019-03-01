#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"
#include "Global.h"

#include <cassert>

MaybeAppDataPtr create_fences(AppDataPtr appData) noexcept
{
    assert(appData->fences.empty());
    
    appData->fences.resize(numConcurrentResources);
    
    for (uint32_t i{0}; i < numConcurrentResources; ++i)
    {
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        
        if (vkCreateFence(appData->device, &info, nullptr, &appData->fences[i]) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to create fence", std::move(appData)});
        
#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_FENCE,
                                    reinterpret_cast<uint64_t>(appData->fences[i]),
                                    std::string{"fence " + std::to_string(i)}.c_str());
#endif
    }
    
    return std::move(appData);
}