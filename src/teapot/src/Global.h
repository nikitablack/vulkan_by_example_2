#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

extern bool framebufferResized;
extern bool solidMode;
extern float tesselationLevel;
static constexpr uint32_t numConcurrentResources{3};
static constexpr uint32_t shaderDataSize{4};