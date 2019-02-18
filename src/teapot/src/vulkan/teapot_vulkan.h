#pragma once

#include "AppData.h"

MaybeAppDataPtr create_instance(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_logical_device(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_shader_modules(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_surface(AppDataPtr appData) noexcept;
MaybeAppDataPtr get_physical_device(AppDataPtr appData) noexcept;
AppDataPtr get_required_window_extensions(AppDataPtr appData) noexcept;