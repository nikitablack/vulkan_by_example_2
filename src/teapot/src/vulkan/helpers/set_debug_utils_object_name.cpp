#include <vulkan/vulkan.h>

bool set_debug_utils_object_name([[maybe_unused]] VkInstance const instance,
                                 [[maybe_unused]] VkDevice const device,
                                 [[maybe_unused]] VkObjectType const objectType,
                                 [[maybe_unused]] uint64_t const objectHandle,
                                 [[maybe_unused]] char const * const pObjectName) noexcept
{

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    auto const func{reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance,
                                                                                             "vkSetDebugUtilsObjectNameEXT"))};
    
    if (func == nullptr)
        return false;
    
    VkDebugUtilsObjectNameInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.pNext = nullptr;
    info.objectType = objectType;
    info.objectHandle = objectHandle;
    info.pObjectName = pObjectName;
    
    return func(device, &info) == VK_SUCCESS;
#else
    return false;
#endif
}