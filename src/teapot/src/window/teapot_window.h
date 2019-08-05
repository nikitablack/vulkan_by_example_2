#pragma once

#include "AppData.h"

struct GLFWwindow;

AppDataPtr create_window(AppDataPtr appData);
void framebuffer_size_callback(GLFWwindow * window, int width, int height) noexcept;
void on_key_press(GLFWwindow * window, int key, int scancode, int action, int mods) noexcept;