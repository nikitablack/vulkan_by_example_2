#include "helpers/set_debug_utils_object_name.h"
#include "teapot_vulkan.h"

#include <cassert>
#include <fstream>

namespace {

std::vector<uint32_t> load_shader(std::string const &fileName)
{
    std::ifstream file{fileName, std::ios::ate | std::ios::binary};
    
    if (!file.is_open())
        throw std::runtime_error{"failed to open shader file " + fileName};
    
    size_t const fileSize{static_cast<size_t>(file.tellg())};
    
    if (fileSize == 0 || fileSize % 4 != 0)
        throw std::runtime_error{"shader file size " + fileName + " should be a multiple of 4"};
    
    std::vector<char> bufferChar(fileSize);
    
    file.seekg(0);
    file.read(bufferChar.data(), static_cast<std::streamsize>(fileSize));
    
    file.close();
    
    std::vector<uint32_t> bufferUint{reinterpret_cast<uint32_t const *>(bufferChar.data()),
                                     reinterpret_cast<uint32_t const *>(bufferChar.data()) + fileSize / 4};
    
    return bufferUint;
}

}

MaybeAppDataPtr create_shader_modules(AppDataPtr appData) noexcept
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
            return tl::make_unexpected(AppDataError{error.what(), std::move(appData)});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->vertexShaderModule) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to create vertex shader module", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->vertexShaderModule),
                                    "vertex shader module");
#endif
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
            return tl::make_unexpected(AppDataError{error.what(), std::move(appData)});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->tessControlShaderModule) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to create tesselation control shader module", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->tessControlShaderModule),
                                    "tesselation control shader module");
#endif
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
            return tl::make_unexpected(AppDataError{error.what(), std::move(appData)});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->tessEvaluationShaderModule) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to create tesselation evaluation shader module", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->tessEvaluationShaderModule),
                                    "tesselation evaluation shader module");
#endif
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
            return tl::make_unexpected(AppDataError{error.what(), std::move(appData)});
        }
        
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = shaderData.size() * 4;
        info.pCode = shaderData.data();
        
        if (vkCreateShaderModule(appData->device, &info, nullptr, &appData->fragmentShaderModule) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to create fragment shader module", std::move(appData)});

#ifdef ENABLE_VULKAN_DEBUG_UTILS
        set_debug_utils_object_name(appData->instance,
                                    appData->device,
                                    VK_OBJECT_TYPE_SHADER_MODULE,
                                    reinterpret_cast<uint64_t>(appData->fragmentShaderModule),
                                    "fragment shader module");
#endif
    }
    
    return std::move(appData);
}