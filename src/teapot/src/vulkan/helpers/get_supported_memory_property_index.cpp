#include "get_supported_memory_property_index.h"

std::optional<uint32_t> get_supported_memory_property_index(VkPhysicalDevice const physicalDevice,
                                                            uint32_t const supportedMemoryTypeBits,
                                                            VkMemoryPropertyFlags const desiredMemoryFlags) noexcept
{
    VkPhysicalDeviceMemoryProperties memProperties{};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    for (uint32_t i{ 0 }; i < memProperties.memoryTypeCount; ++i)
    {
        if ((supportedMemoryTypeBits & (static_cast<uint32_t>(1) << i)) && (memProperties.memoryTypes[i].propertyFlags & desiredMemoryFlags) == desiredMemoryFlags)
        {
            return std::make_optional(i);
        }
    }
    
    return std::nullopt;
}