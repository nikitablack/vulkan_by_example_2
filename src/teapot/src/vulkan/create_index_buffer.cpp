#include "utils/error_message.hpp"
#include "vulkan/helpers/create_local_device_buffer.hpp"
#include "vulkan/helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>

AppDataPtr create_index_buffer(AppDataPtr appData)
{
    assert(!appData->indexBuffer);
    assert(!appData->indexBufferDeviceMemory);
    
    try
    {
        LocalDeviceBufferDataPtr bufferData{std::make_unique<LocalDeviceBufferData>()};
        bufferData->instance = appData->instance;
        bufferData->device = appData->device;
        bufferData->physicalDevice = appData->physicalDevice;
        bufferData->dataSize = sizeof(decltype(appData->teapotData.patches)::value_type) * appData->teapotData.patches.size();
        bufferData->data = appData->teapotData.patches.data();
        bufferData->localDeviceBufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferData->copyQueueFamilyIndex = appData->graphicsFamilyQueueIndex;
        bufferData->copyQueue = appData->graphicsQueue;
        bufferData->localDeviceBufferAccessMask = VK_ACCESS_INDEX_READ_BIT;
        bufferData->localDeviceBufferStageMask = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        
        bufferData = create_local_device_buffer(std::move(bufferData));
        
        appData->indexBuffer = bufferData->buffer;
        appData->indexBufferDeviceMemory = bufferData->bufferDeviceMemory;
    }
    catch(LocalDeviceBufferDataError const & error)
    {
        appData->indexBuffer = error.bufferData.buffer;
        appData->indexBufferDeviceMemory = error.bufferData.bufferDeviceMemory;
        
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to create index buffer"), std::move(*appData.release())});
    }
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->indexBuffer),
                                "index buffer");
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(appData->indexBufferDeviceMemory),
                                "index buffer device memory");
    
    return appData;
}