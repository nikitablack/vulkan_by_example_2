#pragma once

#include "TeapotData.h"

#include <tl/expected.hpp>

#include <memory>
#include <string>

struct GLFWwindow;

struct AppData
{
    TeapotData teapotData{};
    
    GLFWwindow * window{nullptr};
};

using AppDataPtr = std::unique_ptr<AppData>;

struct AppDataError
{
    std::string message{};
    AppDataPtr appData{};
};

using MaybeAppDataPtr = tl::expected<AppDataPtr, AppDataError>;