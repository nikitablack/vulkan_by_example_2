#include "teapot_vulkan.h"
#include "Global.h"

#include <array>
#include <cassert>

namespace
{

void update_set(VkDevice const device,
                VkDescriptorSet const descriptorSet,
                VkBuffer const patchBuffer,
                VkBuffer const projMatrixBuffer,
                VkBuffer const viewMatrixBuffer,
                VkBuffer const modelMatrixBuffer,
                VkDeviceSize const matrixBufferOffset)
{
    {
        VkDescriptorBufferInfo descriptorBufferInfo{};
        descriptorBufferInfo.buffer = patchBuffer;
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = VK_WHOLE_SIZE;
        
        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writeDescriptorSet.pImageInfo = nullptr;
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
        writeDescriptorSet.pTexelBufferView = nullptr;
        
        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
    }
    
    {
        std::array<VkDescriptorBufferInfo, 3> descriptorBufferInfos{};
        
        descriptorBufferInfos[0].buffer = projMatrixBuffer;
        descriptorBufferInfos[0].offset = matrixBufferOffset;
        descriptorBufferInfos[0].range = 16 * shaderDataSize;
        
        descriptorBufferInfos[1].buffer = viewMatrixBuffer;
        descriptorBufferInfos[1].offset = matrixBufferOffset;
        descriptorBufferInfos[1].range = 16 * shaderDataSize;
        
        descriptorBufferInfos[2].buffer = modelMatrixBuffer;
        descriptorBufferInfos[2].offset = matrixBufferOffset;
        descriptorBufferInfos[2].range = 16 * shaderDataSize;
        
        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = nullptr;
        writeDescriptorSet.dstSet = descriptorSet;
        writeDescriptorSet.dstBinding = 1;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorCount = 3;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.pImageInfo = nullptr;
        writeDescriptorSet.pBufferInfo = descriptorBufferInfos.data();
        writeDescriptorSet.pTexelBufferView = nullptr;
        
        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
    }
}

}

MaybeAppDataPtr allocate_and_update_descriptor_sets(AppDataPtr appData) noexcept
{
    assert(appData->descriptorSets.empty());
    
    std::array<VkDescriptorSetLayout, 2> descriptorSetLayouts{appData->descriptorSetLayout, appData->descriptorSetLayout};
    
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.descriptorPool = appData->descriptorPool;
    info.descriptorSetCount = numConcurrentResources;
    info.pSetLayouts = descriptorSetLayouts.data();
    
    appData->descriptorSets.resize(numConcurrentResources);
    
    if (vkAllocateDescriptorSets(appData->device, &info, appData->descriptorSets.data()) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to allocate descriptor set", std::move(appData)});
    
    for (uint32_t i{ 0 }; i < numConcurrentResources; ++i)
    {
        update_set(appData->device,
            appData->descriptorSets[i],
            appData->patchBuffer,
            appData->projMatrixBuffer,
            appData->viewMatrixBuffer,
            appData->modelMatrixBuffer,
            appData->matrixBufferOffset * i);
    }
    
    return std::move(appData);
}