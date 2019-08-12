#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"
#include "Global.hpp"

#include <array>
#include <cassert>

AppDataPtr create_descriptor_pool(AppDataPtr appData)
{
    assert(!appData->descriptorPool);
    
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = numConcurrentResources;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = numConcurrentResources * 3;
    
    VkDescriptorPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.maxSets = numConcurrentResources;
    info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    info.pPoolSizes = poolSizes.data();
    
    if (vkCreateDescriptorPool(appData->device, &info, nullptr, &appData->descriptorPool) != VK_SUCCESS)
        AppDataError{ERROR_MESSAGE("failed to create descriptor pool"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DESCRIPTOR_POOL,
                                reinterpret_cast<uint64_t>(appData->descriptorPool),
                                "descriptor pool");
    
    return appData;
}