#include "utils/error_message.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>

AppDataPtr create_logical_device(AppDataPtr appData)
{
    assert(!appData->device);
    
    std::vector<uint32_t> queueIndices{appData->graphicsFamilyQueueIndex};
    std::vector<std::vector<float>> queueNumAndPriorities{{1.0f}};
    
    if (appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        queueIndices.push_back(appData->presentFamilyQueueIndex);
        queueNumAndPriorities.push_back({1.0f});
    }
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    queueCreateInfos.reserve(queueIndices.size());
    
    for (size_t i{0}; i < queueIndices.size(); ++i)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.flags = 0;
        queueCreateInfo.queueFamilyIndex = queueIndices[i];
        queueCreateInfo.queueCount = static_cast<uint32_t>(queueNumAndPriorities[i].size());
        queueCreateInfo.pQueuePriorities = queueNumAndPriorities[i].data();
        
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};
    physicalDeviceFeatures.tessellationShader = VK_TRUE;
    physicalDeviceFeatures.fillModeNonSolid = VK_TRUE;
    
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(appData->deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = appData->deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    
    if (vkCreateDevice(appData->physicalDevice, &deviceCreateInfo, nullptr, &appData->device) != VK_SUCCESS)
        throw AppDataError{ERROR_MESSAGE("failed to create logical device"), std::move(*appData.release())};
    
    return appData;
}