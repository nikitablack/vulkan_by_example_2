#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <cassert>

MaybeAppDataPtr create_semaphores(AppDataPtr appData) noexcept
{
    assert(!appData->imageAvailableSemaphore);
    assert(!appData->graphicsFinishedSemaphore);
    
    VkSemaphoreCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    
    if (vkCreateSemaphore(appData->device, &info, nullptr, &appData->imageAvailableSemaphore) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create image available semaphore", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_SEMAPHORE,
                                reinterpret_cast<uint64_t>(appData->imageAvailableSemaphore),
                                "image available semaphore");
#endif
    
    if (vkCreateSemaphore(appData->device, &info, nullptr, &appData->graphicsFinishedSemaphore) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create graphics finished semaphore", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_SEMAPHORE,
                                reinterpret_cast<uint64_t>(appData->graphicsFinishedSemaphore),
                                "graphics finished semaphore");
#endif
    
    if(appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        if (vkCreateSemaphore(appData->device, &info, nullptr, &appData->queueOwnershipChangedSemaphore) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to create queue ownership changed semaphore", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SEMAPHORE,
                                    reinterpret_cast<uint64_t>(appData->queueOwnershipChangedSemaphore),
                                    "queue ownership changed semaphore");
#endif
    }
    
    return std::move(appData);
}