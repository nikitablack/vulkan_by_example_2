#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>

AppDataPtr create_semaphores(AppDataPtr appData)
{
    assert(!appData->imageAvailableSemaphore);
    assert(!appData->graphicsFinishedSemaphore);
    
    VkSemaphoreCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    
    if (vkCreateSemaphore(appData->device, &info, nullptr, &appData->imageAvailableSemaphore) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create image available semaphore"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_SEMAPHORE,
                                reinterpret_cast<uint64_t>(appData->imageAvailableSemaphore),
                                "image available semaphore");
    
    if (vkCreateSemaphore(appData->device, &info, nullptr, &appData->graphicsFinishedSemaphore) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create graphics finished semaphore"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_SEMAPHORE,
                                reinterpret_cast<uint64_t>(appData->graphicsFinishedSemaphore),
                                "graphics finished semaphore");
    
    if(appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        if (vkCreateSemaphore(appData->device, &info, nullptr, &appData->queueOwnershipChangedSemaphore) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create queue ownership changed semaphore"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SEMAPHORE,
                                    reinterpret_cast<uint64_t>(appData->queueOwnershipChangedSemaphore),
                                    "queue ownership changed semaphore");
    }
    
    return appData;
}