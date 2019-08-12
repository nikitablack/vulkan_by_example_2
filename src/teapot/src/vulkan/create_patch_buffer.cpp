#include "utils/error_message.hpp"
#include "vulkan/helpers/create_local_device_buffer.hpp"
#include "vulkan/helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>

AppDataPtr create_patch_buffer(AppDataPtr appData)
{
    assert(!appData->patchBuffer);
    assert(!appData->patchBufferDeviceMemory);
    assert(appData->teapotData.patchesTransforms.size() / 16 == appData->teapotData.patchesColors.size() / 4);
    
    std::vector<float> patchData{};
    patchData.reserve(appData->teapotData.patchesTransforms.size() + appData->teapotData.patchesColors.size());
    
    using diffType = decltype(patchData)::difference_type;
    
    for(size_t i{0}; i < appData->teapotData.patchesTransforms.size() / 16; ++i)
    {
        patchData.insert(patchData.end(), appData->teapotData.patchesTransforms.begin() + static_cast<diffType>(i * 16), appData->teapotData.patchesTransforms.begin() + static_cast<diffType>(i * 16 + 16));
        patchData.insert(patchData.end(), appData->teapotData.patchesColors.begin() + static_cast<diffType>(i * 4), appData->teapotData.patchesColors.begin() + static_cast<diffType>(i * 4 + 4));
    }
    
    try
    {
        LocalDeviceBufferDataPtr bufferData{std::make_unique<LocalDeviceBufferData>()};
        bufferData->instance = appData->instance;
        bufferData->device = appData->device;
        bufferData->physicalDevice = appData->physicalDevice;
        bufferData->dataSize = sizeof(decltype(patchData)::value_type) * patchData.size();
        bufferData->data = appData->teapotData.patches.data();
        bufferData->localDeviceBufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        bufferData->copyQueueFamilyIndex = appData->graphicsFamilyQueueIndex;
        bufferData->copyQueue = appData->graphicsQueue;
        bufferData->localDeviceBufferAccessMask = VK_ACCESS_SHADER_READ_BIT;
        bufferData->localDeviceBufferStageMask = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
        
        bufferData = create_local_device_buffer(std::move(bufferData));
    
        appData->patchBuffer = bufferData->buffer;
        appData->patchBufferDeviceMemory = bufferData->bufferDeviceMemory;
    }
    catch(LocalDeviceBufferDataError error)
    {
        appData->patchBuffer = error.bufferData.buffer;
        appData->patchBufferDeviceMemory = error.bufferData.bufferDeviceMemory;
    
        std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to create patch buffer"), *appData});
    }
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_BUFFER,
                                reinterpret_cast<uint64_t>(appData->patchBuffer),
                                "patch buffer");
    
    set_debug_utils_object_name(appData->instance,
                                appData->device,
                                VK_OBJECT_TYPE_DEVICE_MEMORY,
                                reinterpret_cast<uint64_t>(appData->patchBufferDeviceMemory),
                                "patch buffer device memory");
    
    return appData;
}