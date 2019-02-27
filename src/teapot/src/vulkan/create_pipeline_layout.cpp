#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"
#include "Global.h"

#include <cassert>

MaybeAppDataPtr create_pipeline_layout(AppDataPtr appData) noexcept
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
        return tl::make_unexpected(AppDataError{"failed to create pipeline layout", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_PIPELINE_LAYOUT,
                                reinterpret_cast<uint64_t>(appData->pipelineLayout),
                                "pipeline layout");
#endif
    
    return std::move(appData);
}