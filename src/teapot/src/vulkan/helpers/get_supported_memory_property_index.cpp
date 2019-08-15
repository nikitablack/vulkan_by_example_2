#include "utils/error_message.hpp"
#include "get_supported_memory_property_index.hpp"

#include <stdexcept>

uint32_t get_supported_memory_property_index(VkPhysicalDevice const physicalDevice,
                                             uint32_t const supportedMemoryTypeBits,
                                             VkMemoryPropertyFlags const desiredMemoryFlags)
{
    VkPhysicalDeviceMemoryProperties memProperties{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    for (uint32_t i{ 0 }; i < memProperties.memoryTypeCount; ++i)
    {
        if ((supportedMemoryTypeBits & (static_cast<uint32_t>(1) << i)) && (memProperties.memoryTypes[i].propertyFlags & desiredMemoryFlags) == desiredMemoryFlags)
            return i;
    }
    
    throw std::runtime_error{ERROR_MESSAGE("failed to find memory property index")};
}