#include "teapot_vulkan.h"

#include <cassert>

AppDataPtr get_device_qeues(AppDataPtr appData) noexcept
{
	vkGetDeviceQueue(appData->device, appData->graphicsFamilyQueueIndex, 0, &appData->graphicsQueue);
	vkGetDeviceQueue(appData->device, appData->presentFamilyQueueIndex, 0, &appData->presentQueue);
	
	return std::move(appData);
}