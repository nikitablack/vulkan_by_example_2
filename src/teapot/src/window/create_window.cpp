#include "create_window.h"
#include "framebuffer_size_callback.h"
#include "on_key_press.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

MaybeAppDataPtr create_window(AppDataPtr appData) noexcept
{
	if (!glfwInit())
		return tl::make_unexpected("failed to init glfw");
	
	if (!glfwVulkanSupported())
		return tl::make_unexpected("vulkan is not supported");
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	
	GLFWwindow * const window{glfwCreateWindow(800, 600, "Teapot", nullptr, nullptr)};
	if(!window)
		return tl::make_unexpected("failed to create window");
	
	appData->window = window;
	
	glfwSetKeyCallback(appData->window, &on_key_press);
	glfwSetWindowSizeLimits(appData->window, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetFramebufferSizeCallback(appData->window, &framebuffer_size_callback);
	
	return std::move(appData);
}