#pragma once

#include "AppData.hpp"

AppDataPtr create_instance(AppDataPtr appData);
AppDataPtr create_logical_device(AppDataPtr appData);
AppDataPtr create_shader_modules(AppDataPtr appData);
AppDataPtr create_surface(AppDataPtr appData);
AppDataPtr get_physical_device(AppDataPtr appData);
AppDataPtr get_required_window_extensions(AppDataPtr appData) noexcept;