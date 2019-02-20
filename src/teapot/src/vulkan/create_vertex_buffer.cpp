#include "helpers/create_local_device_buffer.h"
#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <cassert>

MaybeAppDataPtr create_vertex_buffer(AppDataPtr appData) noexcept
{
    assert(!appData->vertexBuffer);
    
    LocalDeviceBufferDataPtr bufferData{std::make_unique<LocalDeviceBufferData>()};
    bufferData->instance = appData->instance;
    bufferData->device = appData->device;
    bufferData->physicalDevice = appData->physicalDevice;
    bufferData->dataSize = sizeof(decltype(appData->teapotData.points)::value_type) * appData->teapotData.points.size();
    bufferData->data = appData->teapotData.points.data();
    bufferData->localDeviceBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferData->copyQueueFamilyIndex = appData->graphicsFamilyQueueIndex;
    bufferData->copyQueue = appData->graphicsQueue;
    bufferData->localDeviceBufferAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    bufferData->localDeviceBufferStageMask = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    
    auto mbBufferData{create_local_device_buffer(std::move(bufferData))};
    
    if (!mbBufferData)
        return tl::make_unexpected(mbBufferData.error());
    
    bufferData = std::move(*mbBufferData);
    
    appData->vertexBuffer = bufferData->buffer;
    appData->vertexBufferDeviceMemory = bufferData->bufferDeviceMemory;

#ifdef ENABLE_VULKAN_DEBUG_UTILS
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->vertexBuffer),
                                "vertex buffer");
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(appData->vertexBufferDeviceMemory),
                                "vertex buffer device memory");
#endif
    
    return std::move(appData);
}