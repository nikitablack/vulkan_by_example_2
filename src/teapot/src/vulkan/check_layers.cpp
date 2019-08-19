#include "utils/error_message.hpp"
#include "teapot_vulkan.hpp"

#include <cstring>

AppDataPtr check_layers(AppDataPtr appData)
{
    uint32_t propertyCount;
    
    if(vkEnumerateInstanceLayerProperties(&propertyCount, nullptr) != VK_SUCCESS)
        throw AppDataError(ERROR_MESSAGE("failed to enumerate layer properties"), std::move(*appData.release()));
    
    std::vector<VkLayerProperties> properties(propertyCount);
    
    if(vkEnumerateInstanceLayerProperties(&propertyCount, properties.data()) != VK_SUCCESS)
        throw AppDataError(ERROR_MESSAGE("failed to enumerate layer properties"), std::move(*appData.release()));
    
    for(char const * const & layer : appData->layers)
    {
        for(auto const & property : properties)
        {
            if (strcmp(layer, property.layerName) == 0)
                goto cnt;
        }
        
        throw AppDataError(ERROR_MESSAGE("layer \"" + std::string{layer} + "\" is not supported"), std::move(*appData.release()));
        
        cnt:;
    }
    
    return appData;
}