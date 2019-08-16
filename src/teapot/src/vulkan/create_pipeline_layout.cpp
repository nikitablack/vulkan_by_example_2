#include "utils/error_message.hpp"
#include "helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"
#include "Global.hpp"

#include <cassert>

AppDataPtr create_pipeline_layout(AppDataPtr appData)
{
    assert(!appData->pipelineLayout);
    
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 1 * shaderDataSize;
    
    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.setLayoutCount = 1;
    info.pSetLayouts = &appData->descriptorSetLayout;
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &pushConstantRange;
    
    if (vkCreatePipelineLayout(appData->device, &info, nullptr, &appData->pipelineLayout) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create pipeline layout"), std::move(*appData.release())};
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                                reinterpret_cast<uint64_t>(appData->pipelineLayout),
                                "pipeline layout");
    
    return appData;
}