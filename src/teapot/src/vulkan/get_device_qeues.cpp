#include "teapot_vulkan.hpp"

AppDataPtr get_device_queues(AppDataPtr appData) noexcept
{
    vkGetDeviceQueue(appData->device, appData->graphicsFamilyQueueIndex, 0, &appData->graphicsQueue);
    vkGetDeviceQueue(appData->device, appData->presentFamilyQueueIndex, 0, &appData->presentQueue);
    
    return appData;
}