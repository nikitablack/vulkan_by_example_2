#include "teapot_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <algorithm>
#include <limits>

AppDataPtr get_surface_extent(AppDataPtr appData) noexcept
{
	int windowWidth{0};
	int windowHeight{0};
	glfwGetFramebufferSize(appData->window, &windowWidth, &windowHeight);
	
	VkExtent2D swapChainExtent{};
	if (appData->surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
	{
		swapChainExtent.width = std::max(appData->surfaceCapabilities.minImageExtent.width, std::min(appData->surfaceCapabilities.maxImageExtent.width, static_cast<uint32_t>(windowWidth)));
		swapChainExtent.height = std::max(appData->surfaceCapabilities.minImageExtent.height, std::min(appData->surfaceCapabilities.maxImageExtent.height, static_cast<uint32_t>(windowHeight)));
	}
	else
		swapChainExtent = appData->surfaceCapabilities.currentExtent;
	
	appData->surfaceExtent = swapChainExtent;
	
	return std::move(appData);
}