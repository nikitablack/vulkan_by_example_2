#pragma once

#include "AppData.hpp"

AppDataPtr allocate_and_update_descriptor_sets(AppDataPtr appData);
AppDataPtr check_instance_extensions(AppDataPtr appData);
AppDataPtr check_layers(AppDataPtr appData);
void clean(AppData && appData) noexcept;
AppDataPtr create_descriptor_pool(AppDataPtr appData);
AppDataPtr create_descriptor_set_layout(AppDataPtr appData);
AppDataPtr create_framebuffers(AppDataPtr appData);
AppDataPtr create_index_buffer(AppDataPtr appData);
AppDataPtr create_instance(AppDataPtr appData);
AppDataPtr create_logical_device(AppDataPtr appData);
AppDataPtr create_matrix_buffers(AppDataPtr appData);
AppDataPtr create_patch_buffer(AppDataPtr appData);
AppDataPtr create_pipeline_layout(AppDataPtr appData);
AppDataPtr create_pipelines(AppDataPtr appData);
AppDataPtr create_render_pass(AppDataPtr appData);
AppDataPtr create_shader_modules(AppDataPtr appData);
AppDataPtr create_surface(AppDataPtr appData);
AppDataPtr create_swapchain(AppDataPtr appData);
AppDataPtr create_vertex_buffer(AppDataPtr appData);
AppDataPtr get_device_queues(AppDataPtr appData) noexcept;
AppDataPtr get_physical_device(AppDataPtr appData);
AppDataPtr get_required_window_extensions(AppDataPtr appData) noexcept;
AppDataPtr get_swapchain_images_and_views(AppDataPtr appData);