#include "utils/error_message.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>

AppDataPtr create_instance(AppDataPtr appData)
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
        throw AppDataError{ERROR_MESSAGE("failed to create instance"), *appData};
    
    return appData;
}