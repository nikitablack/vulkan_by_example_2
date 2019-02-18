#include "teapot_vulkan.h"

#include <iostream>
#include <string>

namespace {

std::string string_VkObjectType(VkObjectType const input_value)
{
    switch ((VkObjectType) input_value)
    {
        case VK_OBJECT_TYPE_QUERY_POOL:
            return "VK_OBJECT_TYPE_QUERY_POOL";
        case VK_OBJECT_TYPE_OBJECT_TABLE_NVX:
            return "VK_OBJECT_TYPE_OBJECT_TABLE_NVX";
        case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
            return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
        case VK_OBJECT_TYPE_SEMAPHORE:
            return "VK_OBJECT_TYPE_SEMAPHORE";
        case VK_OBJECT_TYPE_SHADER_MODULE:
            return "VK_OBJECT_TYPE_SHADER_MODULE";
        case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
            return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
        case VK_OBJECT_TYPE_SAMPLER:
            return "VK_OBJECT_TYPE_SAMPLER";
        case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX:
            return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX";
        case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
            return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT";
        case VK_OBJECT_TYPE_IMAGE:
            return "VK_OBJECT_TYPE_IMAGE";
        case VK_OBJECT_TYPE_UNKNOWN:
            return "VK_OBJECT_TYPE_UNKNOWN";
        case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
            return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
        case VK_OBJECT_TYPE_COMMAND_BUFFER:
            return "VK_OBJECT_TYPE_COMMAND_BUFFER";
        case VK_OBJECT_TYPE_BUFFER:
            return "VK_OBJECT_TYPE_BUFFER";
        case VK_OBJECT_TYPE_SURFACE_KHR:
            return "VK_OBJECT_TYPE_SURFACE_KHR";
        case VK_OBJECT_TYPE_INSTANCE:
            return "VK_OBJECT_TYPE_INSTANCE";
        case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
            return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT";
        case VK_OBJECT_TYPE_IMAGE_VIEW:
            return "VK_OBJECT_TYPE_IMAGE_VIEW";
        case VK_OBJECT_TYPE_DESCRIPTOR_SET:
            return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
        case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
            return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
        case VK_OBJECT_TYPE_COMMAND_POOL:
            return "VK_OBJECT_TYPE_COMMAND_POOL";
        case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
            return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
        case VK_OBJECT_TYPE_DISPLAY_KHR:
            return "VK_OBJECT_TYPE_DISPLAY_KHR";
        case VK_OBJECT_TYPE_BUFFER_VIEW:
            return "VK_OBJECT_TYPE_BUFFER_VIEW";
        case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
            return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
        case VK_OBJECT_TYPE_FRAMEBUFFER:
            return "VK_OBJECT_TYPE_FRAMEBUFFER";
        case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
            return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE";
        case VK_OBJECT_TYPE_PIPELINE_CACHE:
            return "VK_OBJECT_TYPE_PIPELINE_CACHE";
        case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
            return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
        case VK_OBJECT_TYPE_DEVICE_MEMORY:
            return "VK_OBJECT_TYPE_DEVICE_MEMORY";
        case VK_OBJECT_TYPE_FENCE:
            return "VK_OBJECT_TYPE_FENCE";
        case VK_OBJECT_TYPE_QUEUE:
            return "VK_OBJECT_TYPE_QUEUE";
        case VK_OBJECT_TYPE_DEVICE:
            return "VK_OBJECT_TYPE_DEVICE";
        case VK_OBJECT_TYPE_RENDER_PASS:
            return "VK_OBJECT_TYPE_RENDER_PASS";
        case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
            return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
        case VK_OBJECT_TYPE_EVENT:
            return "VK_OBJECT_TYPE_EVENT";
        case VK_OBJECT_TYPE_PIPELINE:
            return "VK_OBJECT_TYPE_PIPELINE";
        default:
            return "Unhandled VkObjectType";
    }
}

}

VkBool32 vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT const messageSeverity,
                               VkDebugUtilsMessageTypeFlagsEXT const messageType,
                               VkDebugUtilsMessengerCallbackDataEXT const * const pCallbackData, void * const)
{
    std::string message{};
    message.reserve(5000);
    
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        message += "VERBOSE : ";
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        message += "INFO : ";
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        message += "WARNING : ";
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        message += "ERROR : ";
    
    if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    {
        message += "GENERAL";
    }
    else
    {
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
            message += "VALIDATION";
        
        if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        {
            if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
                message += "|";
            
            message += "PERFORMANCE";
        }
    }
    
    message += " - Message Id Number: " + std::to_string(pCallbackData->messageIdNumber) + " | Message Id Name: " +
               (pCallbackData->pMessageIdName ? pCallbackData->pMessageIdName : "NULL") + "\n\t" +
               pCallbackData->pMessage + "\n";
    
    if (pCallbackData->objectCount > 0)
    {
        message += "\n\tObjects - " + std::to_string(pCallbackData->objectCount) + "\n";
        
        for (uint32_t object = 0; object < pCallbackData->objectCount; ++object)
        {
            if (pCallbackData->pObjects[object].pObjectName != nullptr &&
                std::string{pCallbackData->pObjects[object].pObjectName}.size() > 0)
            {
                message += "\t\tObject[" + std::to_string(object) + "] - " +
                           string_VkObjectType(pCallbackData->pObjects[object].objectType) + ", Handle " +
                           std::to_string(pCallbackData->pObjects[object].objectHandle) + ", Name \"" +
                           pCallbackData->pObjects[object].pObjectName + "\"\n";
            }
            else
            {
                message += "\t\tObject[" + std::to_string(object) + "] - " +
                           string_VkObjectType(pCallbackData->pObjects[object].objectType) + ", Handle " +
                           std::to_string(pCallbackData->pObjects[object].objectHandle) + "\n";
            }
        }
    }
    
    if (pCallbackData->cmdBufLabelCount > 0)
    {
        message += "\n\tCommand Buffer Labels - " + std::to_string(pCallbackData->cmdBufLabelCount) + "\n";
        
        for (uint32_t cmd_buf_label = 0; cmd_buf_label < pCallbackData->cmdBufLabelCount; ++cmd_buf_label)
        {
            message += "\t\tLabel[" + std::to_string(cmd_buf_label) + "] - " +
                       pCallbackData->pCmdBufLabels[cmd_buf_label].pLabelName + " { " +
                       std::to_string(pCallbackData->pCmdBufLabels[cmd_buf_label].color[0]) + ", " +
                       std::to_string(pCallbackData->pCmdBufLabels[cmd_buf_label].color[1]) + ", " +
                       std::to_string(pCallbackData->pCmdBufLabels[cmd_buf_label].color[1]) + ", " +
                       std::to_string(pCallbackData->pCmdBufLabels[cmd_buf_label].color[3]) + "}\n";
        }
    }
    
    std::cerr << message << "\n";
    
    return false;
}