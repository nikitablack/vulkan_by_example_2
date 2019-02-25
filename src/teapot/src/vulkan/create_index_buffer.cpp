#include "helpers/create_local_device_buffer.h"
#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <cassert>

MaybeAppDataPtr create_index_buffer(AppDataPtr appData) noexcept
{
    assert(!appData->indexBuffer);
    assert(!appData->indexBufferDeviceMemory);
    
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
    
    auto mbBufferData{create_local_device_buffer(std::move(bufferData))};
    
    if (!mbBufferData)
    {
        bufferData = std::move(mbBufferData.error().bufferData);
        
        appData->indexBuffer = bufferData->buffer;
        appData->indexBufferDeviceMemory = bufferData->bufferDeviceMemory;
        
        return tl::make_unexpected(AppDataError{mbBufferData.error().message, std::move(appData)});
    }
    
    bufferData = std::move(*mbBufferData);
    
    appData->indexBuffer = bufferData->buffer;
    appData->indexBufferDeviceMemory = bufferData->bufferDeviceMemory;

#ifdef ENABLE_VULKAN_DEBUG_UTILS
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
#endif
    
    return std::move(appData);
}