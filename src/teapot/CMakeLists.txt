cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/bin/glslangValidator" CACHE FILEPATH "Path to glslangValidator executable")

find_package(Vulkan REQUIRED)

add_executable(teapot "")

target_sources(teapot PRIVATE
        src/utils/error_message.hpp

        src/vulkan/helpers/create_local_device_buffer.cpp
        src/vulkan/helpers/create_local_device_buffer.hpp
        src/vulkan/helpers/get_supported_memory_property_index.hpp
        src/vulkan/helpers/get_supported_memory_property_index.cpp
        src/vulkan/helpers/set_debug_utils_object_name.hpp
        src/vulkan/helpers/set_debug_utils_object_name.cpp

        src/vulkan/allocate_and_update_descriptor_sets.cpp
        src/vulkan/clean.cpp
        src/vulkan/create_descriptor_pool.cpp
        src/vulkan/create_descriptor_set_layout.cpp
        src/vulkan/create_framebuffers.cpp
        src/vulkan/create_index_buffer.cpp
        src/vulkan/create_instance.cpp
        src/vulkan/create_logical_device.cpp
        src/vulkan/create_matrix_buffers.cpp
        src/vulkan/create_patch_buffer.cpp
        src/vulkan/create_pipeline_layout.cpp
        src/vulkan/create_pipelines.cpp
        src/vulkan/create_render_pass.cpp
        src/vulkan/create_shader_modules.cpp
        src/vulkan/create_surface.cpp
        src/vulkan/create_swapchain.cpp
        src/vulkan/create_vertex_buffer.cpp
        src/vulkan/get_device_qeues.cpp
        src/vulkan/get_physical_device.cpp
        src/vulkan/get_required_window_extensions.cpp
        src/vulkan/get_swapchain_images_and_views.cpp
        src/vulkan/teapot_vulkan.hpp

        src/window/create_window.cpp
        src/window/framebuffer_size_callback.cpp
        src/window/on_key_press.cpp
        src/window/window.hpp

        src/AppData.hpp
        src/Global.hpp
        src/main.cpp
        src/TeapotData.hpp
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CXXCOMPILEOPTIONS -Werror -pedantic-errors -Wall -ansi -pedantic -Wextra -Wshadow -Weffc++ -Wconversion -Wsign-conversion)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    string(REGEX REPLACE " /W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CXXCOMPILEOPTIONS /WX /W4)
endif ()

set_target_properties(teapot PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )

target_compile_options(teapot PRIVATE ${CXXCOMPILEOPTIONS})

target_compile_definitions(teapot PRIVATE ENABLE_VULKAN_DEBUG_UTILS)

target_link_libraries(teapot PRIVATE
        eigen
        glfw
        ${Vulkan_LIBRARIES}
        )

target_include_directories(teapot PRIVATE src)
target_include_directories(teapot SYSTEM PRIVATE ${Vulkan_INCLUDE_DIRS})

function(addCompileShadersCommand)
    cmake_parse_arguments(addCompileShadersCommand "" "TARGET_NAME;OUTPUT_DIR" "SHADERS" ${ARGN})

    if (NOT addCompileShadersCommand_TARGET_NAME)
        message(FATAL_ERROR "Provide unique target name")
    endif ()

    if (NOT addCompileShadersCommand_SHADERS)
        message(FATAL_ERROR "At least one shader file name should be provided")
    endif ()

    set(FILE_NAMES "")
    set(OUTPUT_PATHS "")

    foreach (shader ${addCompileShadersCommand_SHADERS})
        get_filename_component(VAR ${shader} NAME_WE)
        list(APPEND FILE_NAMES ${VAR})
        list(APPEND OUTPUT_PATHS ${addCompileShadersCommand_OUTPUT_DIR}/${VAR}.spv)

        add_custom_command(OUTPUT ${addCompileShadersCommand_OUTPUT_DIR}/${VAR}.spv
                COMMAND ${GLSL_VALIDATOR} -V ${shader} -o ${addCompileShadersCommand_OUTPUT_DIR}/${VAR}.spv
                DEPENDS ${shader}
                )
    endforeach ()

    add_custom_target(${addCompileShadersCommand_TARGET_NAME}
            DEPENDS ${OUTPUT_PATHS}
            COMMENT "Compiling ${FILE_NAMES}"
            )

    add_dependencies(teapot ${addCompileShadersCommand_TARGET_NAME})
endfunction()

addCompileShadersCommand(TARGET_NAME shaders
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin
        SHADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/VertexShader.vert
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/TesselationControlShader.tesc
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/TesselationEvaluationShader.tese
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/FragmentShader.frag
        )