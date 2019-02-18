#include "vulkan/create_instance.h"
#include "vulkan/create_logical_device.h"
#include "vulkan/create_shader_modules.h"
#include "vulkan/create_surface.h"
#include "vulkan/get_physical_device.h"
#include "vulkan/get_required_window_extensions.h"
#include "window/create_window.h"
#include "AppData.h"
#include "Global.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>

bool framebufferResized{false};
bool solidMode{true};
float tesselationLevel{1.0};

int main()
{
	AppDataPtr appData{std::make_unique<AppData>()};
	
	auto mbAppData{create_window(std::move(appData))
	               .map(get_required_window_extensions)
	               .and_then(create_instance)
	               .and_then(create_surface)
	               .and_then(get_physical_device)
	               .and_then(create_logical_device)
	               .and_then(create_shader_modules)};
	
	if(!mbAppData)
	{
		std::cout << mbAppData.error() << std::endl;
		return 1;
	}
	
	appData = std::move(*mbAppData);
	
	while (!glfwWindowShouldClose(appData->window))
	{
		glfwPollEvents();
	}
	
	glfwTerminate();
	
	return 0;
}