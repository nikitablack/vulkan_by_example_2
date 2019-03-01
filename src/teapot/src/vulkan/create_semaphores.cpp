#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <cassert>

MaybeAppDataPtr create_semaphores(AppDataPtr appData) noexcept
{
    assert(!appData->imageAvailableSemaphore);
    assert(!appData->presentFinishedSemaphore);
    
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
    
    if (vkCreateSemaphore(appData->device, &info, nullptr, &appData->presentFinishedSemaphore) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create present finished semaphore", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_SEMAPHORE,
                                reinterpret_cast<uint64_t>(appData->presentFinishedSemaphore),
                                "present finished semaphore");
#endif
    
    return std::move(appData);
}