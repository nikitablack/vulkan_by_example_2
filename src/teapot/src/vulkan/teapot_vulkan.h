#pragma once

#include "AppData.h"

MaybeAppDataPtr allocate_and_update_descriptor_sets(AppDataPtr appData) noexcept;
AppDataPtr clean(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_debug_utils_messenger(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_descriptor_pool(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_descriptor_set_layout(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_index_buffer(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_instance(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_logical_device(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_debug_utils_messenger(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_matrix_buffers(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_patch_buffer(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_pipeline_layout(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_pipelines(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_render_pass(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_shader_modules(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_surface(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_swap_chain(AppDataPtr appData) noexcept;
MaybeAppDataPtr create_vertex_buffer(AppDataPtr appData) noexcept;
AppDataPtr get_device_qeues(AppDataPtr appData) noexcept;
MaybeAppDataPtr get_physical_device(AppDataPtr appData) noexcept;
AppDataPtr get_required_window_extensions(AppDataPtr appData) noexcept;
AppDataPtr get_surface_extent(AppDataPtr appData) noexcept;

VkBool32 vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT const messageSeverity,
                               VkDebugUtilsMessageTypeFlagsEXT const messageType,
                               VkDebugUtilsMessengerCallbackDataEXT const * const pCallbackData, void * const);