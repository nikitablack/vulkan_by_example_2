#pragma once

#include <vulkan/vulkan.h>

bool set_debug_utils_object_name(VkInstance const instance,
                                 VkDevice const device,
                                 VkObjectType const objectType,
                                 uint64_t const objectHandle,
                                 char const * const pObjectName) noexcept;