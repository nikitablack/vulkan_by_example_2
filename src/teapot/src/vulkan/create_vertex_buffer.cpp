#include "utils/error_message.hpp"
#include "vulkan/helpers/create_local_device_buffer.hpp"
#include "vulkan/helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>

AppDataPtr create_vertex_buffer(AppDataPtr appData)
{
    assert(!appData->vertexBuffer);
    assert(!appData->vertexBufferDeviceMemory);
    
    try
    {
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
        
        bufferData = create_local_device_buffer(std::move(bufferData));
    
        appData->vertexBuffer = bufferData->buffer;
        appData->vertexBufferDeviceMemory = bufferData->bufferDeviceMemory;
    }
    catch(LocalDeviceBufferDataError error)
    {
        appData->vertexBuffer = error.bufferData.buffer;
        appData->vertexBufferDeviceMemory = error.bufferData.bufferDeviceMemory;
    
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to create vertex buffer"), std::move(*appData.release())});
    }
    
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
    
    return appData;
}