#pragma once

#include "AppData.h"

VkBool32 vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT const messageSeverity, VkDebugUtilsMessageTypeFlagsEXT const messageType, VkDebugUtilsMessengerCallbackDataEXT const * const pCallbackData, void * const);