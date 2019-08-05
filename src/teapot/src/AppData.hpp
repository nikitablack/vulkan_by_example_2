#pragma once

#include "TeapotData.hpp"

#include <exception>
#include <memory>
#include <string>

struct GLFWwindow;

struct AppData
{
    TeapotData teapotData{};
    
    GLFWwindow * window{nullptr};
};

using AppDataPtr = std::unique_ptr<AppData>;

struct AppDataError : public std::exception
{
    AppDataError(std::string msg, AppDataPtr ptr) : message{std::move(msg)}, appData{std::move(ptr)}
    {}

    const char * what() const noexcept override
    {
        return message.c_str();
    }

    std::string message{};
    AppDataPtr appData{};
};