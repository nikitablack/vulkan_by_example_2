#pragma once

#include "TeapotData.h"

#include <tl/expected.hpp>

#include <memory>
#include <string>

struct GLFWwindow;

struct AppData
{
	GLFWwindow* window{nullptr};
	bool framebufferResized{false};
	
	TeapotData teapotData{};
};

using AppDataPtr = std::unique_ptr<AppData>;
using MaybeAppDataPtr = tl::expected<AppDataPtr, std::string>;