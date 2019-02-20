#pragma once

#include <vulkan/vulkan.h>

#include <optional>

std::optional<uint32_t> get_supported_memory_property_index(VkPhysicalDevice physicalDevice,
                                                            uint32_t supportedMemoryTypeBits,
                                                            VkMemoryPropertyFlags desiredMemoryFlags) noexcept;