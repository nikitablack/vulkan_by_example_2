#include "teapot_vulkan.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <tuple>

namespace {

void check_required_device_extensions(VkPhysicalDevice const physicalDevice,
                                      std::vector<char const *> const &requiredExtensions)
{
    uint32_t extensionCount{0};
    
    if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error{"failed to get physical device extension properties"};
    
    std::vector<VkExtensionProperties> extensions(extensionCount);
    
    if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data()) != VK_SUCCESS)
        throw std::runtime_error{"failed to get physical device extension properties"};
    
    for (char const * element : requiredExtensions)
    {
        auto lambda{
            [element](VkExtensionProperties const &extensionProp)
            {
                return strcmp(element, extensionProp.extensionName) == 0;
            }};
        
        if (std::find_if(std::begin(extensions), std::end(extensions),lambda ) == std::end(extensions))
            throw std::runtime_error{"extension " + std::string{element} + " is not supported"};
    }
}

void check_device_suitability(VkPhysicalDevice const physicalDevice,
                              std::vector<char const *> const &requiredExtensions)
{
    VkPhysicalDeviceProperties deviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    
    if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        throw std::runtime_error{"the device is not a discrete GPU"};
    
    VkPhysicalDeviceFeatures deviceFeatures{};
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    
    if (!deviceFeatures.tessellationShader)
        throw std::runtime_error{"tesselation shader stages are not supported"};
    
    if (!deviceFeatures.fillModeNonSolid)
        throw std::runtime_error{"wireframe rendering not supported"};
    
    check_required_device_extensions(physicalDevice, requiredExtensions);
}

VkSurfaceFormatKHR get_device_surface_format(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const surface)
{
    uint32_t formatsCount{0};
    
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error{"failed to get physical device surface formats"};
    
    std::vector<VkSurfaceFormatKHR> formats(formatsCount);
    
    if (formatsCount == 0 ||
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, formats.data()) != VK_SUCCESS)
        throw std::runtime_error{"failed to get physical device surface formats"};
    
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        return VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    
    auto lambda{
        [](VkSurfaceFormatKHR const f) {
            return f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }};
    
    if (auto const it = find_if(begin(formats), end(formats), lambda); it != end(formats))
        return VkSurfaceFormatKHR{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    
    return formats[0];
}

VkPresentModeKHR get_device_surface_present_mode(VkPhysicalDevice const physicalDevice, VkSurfaceKHR const surface)
{
    uint32_t presentModesCount{0};
    
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error{"failed to get physical device surface present modes"};
    
    std::vector<VkPresentModeKHR> presentModes(presentModesCount);
    
    if (presentModesCount == 0 ||
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, presentModes.data()) != VK_SUCCESS)
        throw std::runtime_error{"failed to get physical device surface present modes"};
    
    if (auto const it = find(begin(presentModes), end(presentModes), VK_PRESENT_MODE_MAILBOX_KHR); it != end(presentModes))
        return VK_PRESENT_MODE_MAILBOX_KHR;
    
    if (auto const it = find(begin(presentModes), end(presentModes), VK_PRESENT_MODE_IMMEDIATE_KHR); it != end(presentModes))
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

std::tuple<uint32_t, uint32_t> get_device_graphics_and_present_queue_families(VkPhysicalDevice const physicalDevice,
                                                                              VkSurfaceKHR const surface)
{
    uint32_t queueFamilyCount{0};
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
    
    for (size_t i{0}; i < queueFamilies.size(); ++i)
    {
        VkQueueFamilyProperties queueFamily{queueFamilies[i]};
        
        if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
        {
            VkBool32 presentSupported{VK_FALSE};
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, static_cast<uint32_t>(i), surface, &presentSupported);
            
            if (presentSupported)
                return std::make_tuple(static_cast<uint32_t>(i), static_cast<uint32_t>(i));
        }
    }
    
    int graphicsQueueIndex{-1};
    
    for (size_t i{0}; i < queueFamilies.size(); ++i)
    {
        VkQueueFamilyProperties queueFamily{queueFamilies[i]};
        
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueIndex = static_cast<int>(i);
            break;
        }
    }
    
    if (graphicsQueueIndex == -1)
        throw std::runtime_error{"failed to find graphics queue"};
    
    int presentQueueIndex{-1};
    
    for (size_t i{0}; i < queueFamilies.size(); ++i)
    {
        VkQueueFamilyProperties const queueFamily{queueFamilies[i]};
        
        if (queueFamily.queueCount > 0)
        {
            VkBool32 presentSupported{VK_FALSE};
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, static_cast<uint32_t>(i), surface, &presentSupported);
            
            if (presentSupported)
            {
                presentQueueIndex = static_cast<int>(i);
                break;
            }
        }
    }
    
    if (presentQueueIndex == -1)
        throw std::runtime_error{"failed to find present queue"};
    
    return std::make_tuple(static_cast<uint32_t>(graphicsQueueIndex), static_cast<uint32_t>(presentQueueIndex));
}

}

MaybeAppDataPtr get_physical_device(AppDataPtr appData) noexcept
{
    uint32_t deviceCount{0};
    
    if (vkEnumeratePhysicalDevices(appData->instance, &deviceCount, nullptr) != VK_SUCCESS || deviceCount == 0)
        return tl::make_unexpected("failed to find GPUs with Vulkan support");
    
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    
    if (vkEnumeratePhysicalDevices(appData->instance, &deviceCount, physicalDevices.data()) != VK_SUCCESS)
        return tl::make_unexpected("failed to find GPUs with Vulkan support");
    
    for (VkPhysicalDevice const d : physicalDevices)
    {
        try
        {
            check_device_suitability(d, appData->deviceExtensions);
            auto const surfaceFormat{get_device_surface_format(d, appData->surface)};
            auto const surfacePresentMode{get_device_surface_present_mode(d, appData->surface)};
            auto const queueFamilies{get_device_graphics_and_present_queue_families(d, appData->surface)};
            
            appData->physicalDevice = d;
            appData->surfacePresentMode = surfacePresentMode;
            appData->surfaceFormat = surfaceFormat;
            std::tie(appData->graphicsFamilyQueueIndex, appData->presentFamilyQueueIndex) = queueFamilies;
            
            vkGetPhysicalDeviceProperties(appData->physicalDevice, &appData->physicalDeviceProperties);
            
            return std::move(appData);
        }
        catch (std::runtime_error const & /*error*/)
        {
            continue;
        }
    }
    
    return tl::make_unexpected("failed to find suitable device");
}