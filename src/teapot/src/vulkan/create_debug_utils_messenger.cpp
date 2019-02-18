#include "create_debug_utils_messenger.h"

#include <cassert>

MaybeAppDataPtr create_debug_utils_messenger(AppDataPtr appData) noexcept
{
    assert(!appData->debugUtilsMessenger);
	
	constexpr VkDebugUtilsMessageSeverityFlagsEXT const messageSeverity{VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT};
	constexpr VkDebugUtilsMessageTypeFlagsEXT const messageType{VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT};
	
	PFN_vkCreateDebugUtilsMessengerEXT const func{reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(appData->instance, "vkCreateDebugUtilsMessengerEXT"))};
	
	if (func == nullptr)
		return tl::make_unexpected("failed to get debug utils messenger extension function");
	
	VkDebugUtilsMessengerCreateInfoEXT info{};
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info.pNext = nullptr;
	info.flags = 0;
	info.messageSeverity = messageSeverity;
	info.messageType = messageType;
	info.pfnUserCallback = appData->debugCallback;
	info.pUserData = nullptr;
	
	if(func(appData->instance, &info, nullptr, &appData->debugUtilsMessenger) != VK_SUCCESS)
		return tl::make_unexpected("failed to create debug utils messenger");
	
	return std::move(appData);
}