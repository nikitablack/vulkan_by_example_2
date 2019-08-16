#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"
#include "Global.hpp"

#include <cassert>

AppDataPtr allocate_command_buffers(AppDataPtr appData)
{
    assert(appData->graphicsCommandBuffers.empty());
    assert(appData->presentCommandBuffers.empty());
    
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = appData->graphicsCommandPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = numConcurrentResources;
    
    appData->graphicsCommandBuffers.resize(numConcurrentResources);
    
    if (vkAllocateCommandBuffers(appData->device, &info, appData->graphicsCommandBuffers.data()) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to allocate graphics command buffers"), std::move(*appData.release())};
    
    for (uint32_t i{0}; i < numConcurrentResources; ++i)
    {
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_COMMAND_BUFFER,
                                    reinterpret_cast<uint64_t>(appData->graphicsCommandBuffers[i]),
                                    std::string{"graphics command buffer " + std::to_string(i)}.c_str());
    }
    
    if(appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        info.commandPool = appData->presentCommandPool;
        
        appData->presentCommandBuffers.resize(numConcurrentResources);
    
        if (vkAllocateCommandBuffers(appData->device, &info, appData->presentCommandBuffers.data()) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to allocate present command buffers"), std::move(*appData.release())};
    
        for (uint32_t i{0}; i < numConcurrentResources; ++i)
        {
            set_debug_utils_object_name(appData->instance,
                                        appData->device,
                                        VK_OBJECT_TYPE_COMMAND_BUFFER,
                                        reinterpret_cast<uint64_t>(appData->presentCommandBuffers[i]),
                                        std::string{"present command buffer " + std::to_string(i)}.c_str());
        }
    }
    
    return appData;
}