#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"
#include "Global.hpp"

#include <cassert>

AppDataPtr create_fences(AppDataPtr appData)
{
    assert(appData->graphicsFences.empty());
    assert(appData->presentFences.empty());
    
    VkFenceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    appData->graphicsFences.resize(numConcurrentResources);
    
    for (uint32_t i{0}; i < numConcurrentResources; ++i)
    {
        if (vkCreateFence(appData->device, &info, nullptr, &appData->graphicsFences[i]) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create graphics fence"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_FENCE,
                                    reinterpret_cast<uint64_t>(appData->graphicsFences[i]),
                                    std::string{"graphics fence " + std::to_string(i)}.c_str());
    }
    
    if(appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        appData->presentFences.resize(numConcurrentResources);
    
        for (uint32_t i{0}; i < numConcurrentResources; ++i)
        {
            if (vkCreateFence(appData->device, &info, nullptr, &appData->presentFences[i]) != VK_SUCCESS)
                throw AppDataError{ERROR_MESSAGE("failed to create present fence"), std::move(*appData.release())};
            
            set_debug_utils_object_name(appData->instance,
                                        appData->device,
                                        VK_OBJECT_TYPE_FENCE,
                                        reinterpret_cast<uint64_t>(appData->presentFences[i]),
                                        std::string{"present fence " + std::to_string(i)}.c_str());
        }
    }
    
    return appData;
}