#include "create_logical_device.h"

#include <cassert>

MaybeAppDataPtr create_logical_device(AppDataPtr appData) noexcept
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
		queueCreateInfo.queueCount = static_cast<uint32_t>(queueNumAndPriorities.size());
		queueCreateInfo.pQueuePriorities = queueNumAndPriorities[i].data();
	
		queueCreateInfos.push_back(queueCreateInfo);
	}
	
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	physicalDeviceFeatures.tessellationShader = VK_TRUE;
	physicalDeviceFeatures.fillModeNonSolid = VK_TRUE;
	physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
	
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
	
	VkDevice device{VK_NULL_HANDLE};
	if (vkCreateDevice(appData->physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
		return tl::make_unexpected("failed to create logical device");
	
	appData->device = device;
	
	return std::move(appData);
}