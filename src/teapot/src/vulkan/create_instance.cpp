#include "create_instance.h"

#include <cassert>

MaybeAppDataPtr create_instance(AppDataPtr appData) noexcept
{
	assert(!appData->instance);
	
	VkInstanceCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;
	info.pApplicationInfo = nullptr;
	info.enabledLayerCount = static_cast<uint32_t>(appData->layers.size());
	info.ppEnabledLayerNames = appData->layers.data();
	info.enabledExtensionCount = static_cast<uint32_t>(appData->instanceExtensions.size());
	info.ppEnabledExtensionNames = appData->instanceExtensions.data();
	
	VkInstance instance{VK_NULL_HANDLE};
	if (vkCreateInstance(&info, nullptr, &instance) != VK_SUCCESS)
		return tl::make_unexpected("failed to create instance");
	
	appData->instance = instance;
	
	return std::move(appData);
}