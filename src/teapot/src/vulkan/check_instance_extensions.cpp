#include "utils/error_message.hpp"
#include "teapot_vulkan.hpp"

#include <cstring>

AppDataPtr check_instance_extensions(AppDataPtr appData)
{
    uint32_t propertyCount;
    
    if(vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr) != VK_SUCCESS)
        throw AppDataError(ERROR_MESSAGE("failed to enumerate instance extension properties"), std::move(*appData.release()));
    
    std::vector<VkExtensionProperties> properties(propertyCount);
    
    if(vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data()) != VK_SUCCESS)
        throw AppDataError(ERROR_MESSAGE("failed to enumerate instance extension properties"), std::move(*appData.release()));
    
    for(char const * const & extension : appData->instanceExtensions)
    {
        for(auto const & property : properties)
        {
            if (strcmp(extension, property.extensionName) == 0)
                goto cnt;
        }
        
        throw AppDataError(ERROR_MESSAGE("instance extension \"" + std::string{extension} + "\" is not supported"), std::move(*appData.release()));
        
        cnt:;
    }
    
    return appData;
}