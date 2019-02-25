#include "teapot_vulkan.h"

#include <cassert>

MaybeAppDataPtr create_instance(AppDataPtr appData) noexcept
{
    assert(!appData->instance);
    
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.pApplicationInfo = nullptr;
    info.enabledLayerCount = static_cast<uint32_t>(appData->layers.size());
    info.ppEnabledLayerNames = appData->layers.data();
    info.enabledExtensionCount = static_cast<uint32_t>(appData->instanceExtensions.size());
    info.ppEnabledExtensionNames = appData->instanceExtensions.data();
    
    if (vkCreateInstance(&info, nullptr, &appData->instance) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create instance", std::move(appData)});
    
    return std::move(appData);
}