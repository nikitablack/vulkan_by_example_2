#include "utils/error_message.hpp"
#include "vulkan/helpers/set_debug_utils_object_name.hpp"
#include "teapot_vulkan.hpp"

#include <cassert>
#include <cstring>
#include <fstream>

namespace
{

std::vector<uint32_t> load_shader(std::string const &fileName)
{
    std::ifstream file{fileName, std::ios::ate | std::ios::binary};
    
    if (!file.is_open())
        throw std::runtime_error{ERROR_MESSAGE("failed to open shader file " + fileName)};
    
    size_t const fileSize{static_cast<size_t>(file.tellg())};
    
    if (fileSize == 0 || fileSize % 4 != 0)
        throw std::runtime_error{ERROR_MESSAGE("shader file size " + fileName + " should be a nonzero multiple of 4")};
    
    std::vector<char> shaderCode(fileSize);
    
    file.seekg(0);
    file.read(shaderCode.data(), static_cast<std::streamsize>(fileSize));
    
    file.close();
    
    std::vector<uint32_t> spirv(shaderCode.size() / 4);
    memcpy(spirv.data(), shaderCode.data(), shaderCode.size());
    
    return spirv;
}

}

AppDataPtr create_shader_modules(AppDataPtr appData)
{
    assert(!appData->vertexShaderModule);
    assert(!appData->tessControlShaderModule);
    assert(!appData->tessEvaluationShaderModule);
    assert(!appData->fragmentShaderModule);
    
    // vertex shader
    {
        std::vector<uint32_t> shaderData{};
        
        try
        {
            shaderData = load_shader("VertexShader.spv");
        }
        catch (std::runtime_error const &error)
        {
            std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to get vertex shader data"), std::move(*appData.release())});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->vertexShaderModule) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create vertex shader module"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->vertexShaderModule),
                                    "vertex shader module");
    }
    
    // tesselation control shader
    {
        std::vector<uint32_t> shaderData{};
        
        try
        {
            shaderData = load_shader("TesselationControlShader.spv");
        }
        catch (std::runtime_error const &error)
        {
            std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to get tesselation control shader data"), std::move(*appData.release())});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->tessControlShaderModule) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create tesselation control shader module"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->tessControlShaderModule),
                                    "tesselation control shader module");
    }
    
    // tesselation evaluation shader
    {
        std::vector<uint32_t> shaderData{};
        
        try
        {
            shaderData = load_shader("TesselationEvaluationShader.spv");
        }
        catch (std::runtime_error const &error)
        {
            std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to get tesselation evaluation shader data"), std::move(*appData.release())});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->tessEvaluationShaderModule) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create tesselation evaluation shader module"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->tessEvaluationShaderModule),
                                    "tesselation evaluation shader module");
    }
    
    // fragment shader
    {
        std::vector<uint32_t> shaderData{};
        
        try
        {
            shaderData = load_shader("FragmentShader.spv");
        }
        catch (std::runtime_error const &error)
        {
            std::throw_with_nested(AppDataError{ERROR_MESSAGE("failed to get fragment shader data"), std::move(*appData.release())});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->fragmentShaderModule) != VK_SUCCESS)
            throw AppDataError{ERROR_MESSAGE("failed to create fragment shader module"), std::move(*appData.release())};
        
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->fragmentShaderModule),
                                    "fragment shader module");
    }
    
    return appData;
}