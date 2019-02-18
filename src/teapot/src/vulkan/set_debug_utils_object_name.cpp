#include <vk_helpers/get_debug_utils_object_name_info.h>
#include <vk_helpers/set_debug_utils_object_name.h>

#include <cassert>

namespace vk_helpers
{

bool set_debug_utils_object_name(VkInstance const instance,
                                 VkDevice const device,
                                 VkObjectType const objectType,
                                 uint64_t const objectHandle,
                                 char const * const pObjectName)
{
	assert(instance);
	
	auto const func{reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"))};
	
	assert(func);
	
	if (func == nullptr)
		return false;
	
	auto const info{get_debug_utils_object_name_info(objectType,
	                                                 objectHandle,
	                                                 pObjectName)};
	
	return func(device, &info) == VK_SUCCESS;
}

} // namespace vk_helpers