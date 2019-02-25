#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <array>
#include <cassert>

MaybeAppDataPtr create_descriptor_pool(AppDataPtr appData) noexcept
{
    assert(!appData->descriptorPool);
    
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = 3;
    
    VkDescriptorPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.maxSets = 1;
    info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    info.pPoolSizes = poolSizes.data();
    
    if (vkCreateDescriptorPool(appData->device, &info, nullptr, &appData->descriptorPool) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to create descriptor pool", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DESCRIPTOR_POOL,
                                reinterpret_cast<uint64_t>(appData->descriptorPool),
                                "descriptor pool");
#endif
    
    return std::move(appData);
}