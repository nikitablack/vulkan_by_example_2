#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <array>
#include <cassert>

AppDataPtr create_descriptor_set_layout(AppDataPtr appData)
{
    assert(!appData->descriptorSetLayout);
    
    std::array<VkDescriptorSetLayoutBinding, 4> descriptorSetLayoutBindings{};
    
    descriptorSetLayoutBindings[0].binding = 0;
    descriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorSetLayoutBindings[0].descriptorCount = 1;
    descriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    descriptorSetLayoutBindings[0].pImmutableSamplers = nullptr;
    
    descriptorSetLayoutBindings[1].binding = 1;
    descriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBindings[1].descriptorCount = 1;
    descriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    descriptorSetLayoutBindings[1].pImmutableSamplers = nullptr;
    
    descriptorSetLayoutBindings[2].binding = 2;
    descriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBindings[2].descriptorCount = 1;
    descriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    descriptorSetLayoutBindings[2].pImmutableSamplers = nullptr;
    
    descriptorSetLayoutBindings[3].binding = 3;
    descriptorSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBindings[3].descriptorCount = 1;
    descriptorSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    descriptorSetLayoutBindings[3].pImmutableSamplers = nullptr;
    
    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
    info.pBindings = descriptorSetLayoutBindings.data();
    
    if (vkCreateDescriptorSetLayout(appData->device, &info, nullptr, &appData->descriptorSetLayout) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create descriptor set layout"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
                                reinterpret_cast<uint64_t>(appData->descriptorSetLayout),
                                "descriptor set layout");
    
    return appData;
}