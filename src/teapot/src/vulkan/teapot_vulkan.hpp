#pragma once

#include "AppData.hpp"

AppDataPtr check_instance_extensions(AppDataPtr appData);
AppDataPtr check_layers(AppDataPtr appData);
void clean(AppData && appData) noexcept;
AppDataPtr create_index_buffer(AppDataPtr appData);
AppDataPtr create_instance(AppDataPtr appData);
AppDataPtr create_logical_device(AppDataPtr appData);
AppDataPtr create_matrix_buffers(AppDataPtr appData);
AppDataPtr create_patch_buffer(AppDataPtr appData);
AppDataPtr create_shader_modules(AppDataPtr appData);
AppDataPtr create_surface(AppDataPtr appData);
AppDataPtr create_vertex_buffer(AppDataPtr appData);
AppDataPtr get_device_queues(AppDataPtr appData) noexcept;
AppDataPtr get_physical_device(AppDataPtr appData);
AppDataPtr get_required_window_extensions(AppDataPtr appData) noexcept;