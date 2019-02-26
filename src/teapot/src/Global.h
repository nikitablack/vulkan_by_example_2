#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>

extern bool framebufferResized;
extern bool solidMode;
extern float tesselationLevel;
extern uint32_t numConcurrentResources;
static constexpr uint32_t shaderDataSize{4};