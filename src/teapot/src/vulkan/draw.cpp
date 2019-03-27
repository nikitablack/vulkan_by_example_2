#include "teapot_vulkan.h"
#include "Global.h"

#include <Dense>

#include <array>
#include <cmath>

using Matrix4 = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;

namespace
{

Matrix4 perspective(float const fovy, float const aspect, float const zNear, float const zFar)
{
    float const yScale{1.0f / std::tan(fovy / 2)};
    float const xScale{yScale / aspect};
    
    Matrix4 mat{};
    mat << xScale, 0, 0, 0,
        0, yScale, 0, 0,
        0, 0, zFar / (zFar - zNear), 1,
        0, 0, -zNear * zFar / (zFar - zNear), 0;
    
    return mat;
}

void update_projection_matrix(float const aspectRatio, char * const memPtr)
{
    Matrix4 mat{perspective(45.0f *  3.14159265358979323846f / 180.0f, aspectRatio, 0.1f, 100.0f)};

    memcpy(memPtr, mat.data(), shaderDataSize * 16);
}

Matrix4 look_at(Eigen::RowVector3f const & eye, Eigen::RowVector3f const & at, Eigen::RowVector3f const & up)
{
    Eigen::RowVector3f const zAxis{(at - eye).normalized()};
    Eigen::RowVector3f const xAxis{(up.cross(zAxis)).normalized()};
    Eigen::RowVector3f const yAxis{zAxis.cross(xAxis)};
    
    Matrix4 mat;
    mat << xAxis.x(), yAxis.x(), zAxis.x(), 0.0f,
           xAxis.y(), yAxis.y(), zAxis.y(), 0.0f,
           xAxis.z(), yAxis.z(), zAxis.z(), 0.0f,
           xAxis.dot(-eye), yAxis.dot(-eye), zAxis.dot(-eye), 1.0f;
    
    return mat;
}

void update_view_matrix(char * const memPtr)
{
    Matrix4 const mat{look_at({0.0f, 10.0f, 5.5f}, {0.0f, 0.0f, 1.5f}, {0.0f, 0.0f, -1.0f})};
    
    memcpy(memPtr, mat.data(), shaderDataSize * 16);
}

Matrix4 model(uint32_t const n)
{
    Eigen::AngleAxis<float> const aa{static_cast<float>(n) * 0.001f, Eigen::Vector3f{0.0f, 0.0f, 1.0f}};
    Eigen::Matrix3f const r{aa.toRotationMatrix()};
    
    Matrix4 mat{ Matrix4::Identity() };
    mat.row(0) << r.row(0), 0;
    mat.row(1) << r.row(1), 0;
    mat.row(2) << r.row(2), 0;
    
    return mat;
}

void update_model_matrix(uint32_t const n, char * const memPtr)
{
    Matrix4 mat{model(n)};
    
    memcpy(memPtr, mat.data(), shaderDataSize * 16);
}

} // namespace

MaybeAppDataPtr draw(AppDataPtr appData) noexcept
{
    uint32_t imageIndex{};
    if(vkAcquireNextImageKHR(appData->device, appData->swapchain, std::numeric_limits<uint64_t>::max(), appData->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to acquire swap chain image", std::move(appData)});
    
    auto const graphicsFence{appData->graphicsFences[appData->currentResourceIndex]};
    
    if(vkWaitForFences(appData->device, 1, &graphicsFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to wait for graphics fence", std::move(appData)});
    
    if(vkResetFences(appData->device, 1, &graphicsFence) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to reset graphics fence", std::move(appData)});
    
    auto const graphicsCommandBuffer{appData->graphicsCommandBuffers[appData->currentResourceIndex]};
    
    vkResetCommandBuffer(graphicsCommandBuffer, 0);
    
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    
    if(vkBeginCommandBuffer(graphicsCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to begin graphics command buffer", std::move(appData)});
    
    void * mappedMatrixBuffersMemory{nullptr};
    if (vkMapMemory(appData->device, appData->matrixBuffersDeviceMemory, 0, VK_WHOLE_SIZE, 0, &mappedMatrixBuffersMemory) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to map uniform buffer memory", std::move(appData)});

    update_projection_matrix(static_cast<float>(appData->surfaceExtent.width) / static_cast<float>(appData->surfaceExtent.height), static_cast<char *>(mappedMatrixBuffersMemory) + appData->currentResourceIndex * appData->matrixBufferOffset);
    update_view_matrix(static_cast<char *>(mappedMatrixBuffersMemory) + appData->matrixBufferSizeAligned + appData->currentResourceIndex * appData->matrixBufferOffset);
    
    static uint32_t n{0};
    update_model_matrix(n++, static_cast<char *>(mappedMatrixBuffersMemory) + appData->matrixBufferSizeAligned * 2 + appData->currentResourceIndex * appData->matrixBufferOffset);

    VkBufferMemoryBarrier projBufferMemoryBarrier{};
    projBufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    projBufferMemoryBarrier.pNext = nullptr;
    projBufferMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    projBufferMemoryBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
    projBufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    projBufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    projBufferMemoryBarrier.buffer = appData->projMatrixBuffer;
    projBufferMemoryBarrier.offset = appData->currentResourceIndex * appData->matrixBufferOffset;
    projBufferMemoryBarrier.size = shaderDataSize * 16;
    
    VkBufferMemoryBarrier viewBufferMemoryBarrier{projBufferMemoryBarrier};
    viewBufferMemoryBarrier.buffer = appData->viewMatrixBuffer;
    
    VkBufferMemoryBarrier modelBufferMemoryBarrier{projBufferMemoryBarrier};
    modelBufferMemoryBarrier.buffer = appData->modelMatrixBuffer;
    
    std::array<VkBufferMemoryBarrier, 3> barriers{projBufferMemoryBarrier, viewBufferMemoryBarrier, modelBufferMemoryBarrier};

    vkCmdPipelineBarrier(graphicsCommandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT, 0, 0, nullptr, static_cast<uint32_t>(barriers.size()), barriers.data(), 0, nullptr);
    
    vkUnmapMemory(appData->device, appData->matrixBuffersDeviceMemory);
    
    vkCmdPushConstants(graphicsCommandBuffer, appData->pipelineLayout, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, 0, shaderDataSize, &tesselationLevel);
    
    VkClearColorValue clearColorValue{};
    clearColorValue.float32[0] = 0.5f;
    clearColorValue.float32[1] = 0.5f;
    clearColorValue.float32[2] = 0.5f;
    clearColorValue.float32[3] = 1.0f;
    
    VkClearValue clearValue{};
    clearValue.color = clearColorValue;
    
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = appData->renderPass;
    renderPassBeginInfo.framebuffer = appData->framebuffers[imageIndex];
    renderPassBeginInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, appData->surfaceExtent};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;
    
    vkCmdBeginRenderPass(graphicsCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport const viewport{0.0f, 0.0f, static_cast<float>(appData->surfaceExtent.width), static_cast<float>(appData->surfaceExtent.height), 0.0f, 1.0f};
    VkRect2D const scissor{VkOffset2D{0, 0}, VkExtent2D{appData->surfaceExtent.width, appData->surfaceExtent.height}};
    
    vkCmdSetViewport(graphicsCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(graphicsCommandBuffer, 0, 1, &scissor);
    
    if(solidMode)
    {
        vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appData->solidPipeline);
    }
    else
    {
        vkCmdBindPipeline(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appData->wireframePipeline);
    }
    
    VkDeviceSize const offset{0};
    vkCmdBindVertexBuffers(graphicsCommandBuffer, 0, 1, &appData->vertexBuffer, &offset);
    
    vkCmdBindIndexBuffer(graphicsCommandBuffer, appData->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    
    vkCmdBindDescriptorSets(graphicsCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appData->pipelineLayout, 0, 1, &appData->descriptorSets[appData->currentResourceIndex], 0, nullptr);
    
    vkCmdDrawIndexed(graphicsCommandBuffer, static_cast<uint32_t>(appData->teapotData.patches.size()), 1, 0, 0, 0);
    
    vkCmdEndRenderPass(graphicsCommandBuffer);
    
    // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#multiple-queues
    if(appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        VkImageMemoryBarrier releaseImageMemoryBarrier{};
        releaseImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        releaseImageMemoryBarrier.pNext = nullptr;
        releaseImageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        releaseImageMemoryBarrier.dstAccessMask = 0;
        releaseImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        releaseImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        releaseImageMemoryBarrier.srcQueueFamilyIndex = appData->graphicsFamilyQueueIndex;
        releaseImageMemoryBarrier.dstQueueFamilyIndex = appData->presentFamilyQueueIndex;
        releaseImageMemoryBarrier.image = appData->swapchainImages[imageIndex];
        releaseImageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    
        vkCmdPipelineBarrier(graphicsCommandBuffer,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &releaseImageMemoryBarrier);
    }
    
    if(vkEndCommandBuffer(graphicsCommandBuffer) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to end graphics command buffer", std::move(appData)});
    
    VkPipelineStageFlags const waitStage{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &appData->imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &graphicsCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &appData->graphicsFinishedSemaphore;
    
    if(vkQueueSubmit(appData->graphicsQueue, 1, &submitInfo, graphicsFence) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to submit graphics command buffer", std::move(appData)});
    
    // https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples#multiple-queues
    if(appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex)
    {
        auto const presentFence{appData->presentFences[appData->currentResourceIndex]};
    
        if(vkWaitForFences(appData->device, 1, &presentFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to wait for present fence", std::move(appData)});
    
        if(vkResetFences(appData->device, 1, &presentFence) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to reset present fence", std::move(appData)});
        
        auto const presentCommandBuffer{appData->presentCommandBuffers[appData->currentResourceIndex]};
    
        if(vkBeginCommandBuffer(presentCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to begin present command buffer", std::move(appData)});
    
        VkImageMemoryBarrier acquireImageMemoryBarrier{};
        acquireImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        acquireImageMemoryBarrier.pNext = nullptr;
        acquireImageMemoryBarrier.srcAccessMask = 0;
        acquireImageMemoryBarrier.dstAccessMask = 0;
        acquireImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        acquireImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        acquireImageMemoryBarrier.srcQueueFamilyIndex = appData->graphicsFamilyQueueIndex;
        acquireImageMemoryBarrier.dstQueueFamilyIndex = appData->presentFamilyQueueIndex;
        acquireImageMemoryBarrier.image = appData->swapchainImages[imageIndex];
        acquireImageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    
        vkCmdPipelineBarrier(presentCommandBuffer,
                             VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &acquireImageMemoryBarrier);
    
        if(vkEndCommandBuffer(presentCommandBuffer) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to end present command buffer", std::move(appData)});
        
        VkPipelineStageFlags const waitStage2{VK_PIPELINE_STAGE_ALL_COMMANDS_BIT};
    
        VkSubmitInfo submitInfo2{};
        submitInfo2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo2.pNext = nullptr;
        submitInfo2.waitSemaphoreCount = 1;
        submitInfo2.pWaitSemaphores = &appData->graphicsFinishedSemaphore;
        submitInfo2.pWaitDstStageMask = &waitStage2;
        submitInfo2.commandBufferCount = 1;
        submitInfo2.pCommandBuffers = &presentCommandBuffer;
        submitInfo2.signalSemaphoreCount = 1;
        submitInfo2.pSignalSemaphores = &appData->queueOwnershipChangedSemaphore;
    
        if(vkQueueSubmit(appData->presentQueue, 1, &submitInfo2, presentFence) != VK_SUCCESS)
            return tl::make_unexpected(AppDataError{"failed to submit present command buffer", std::move(appData)});
    }
    
    VkSemaphore const waitSemaphore{appData->graphicsFamilyQueueIndex != appData->presentFamilyQueueIndex ? appData->queueOwnershipChangedSemaphore : appData->graphicsFinishedSemaphore};
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &waitSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &appData->swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;
    
    auto const res{vkQueuePresentKHR(appData->presentQueue, &presentInfo)};
    if(res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized)
    {
        framebufferResized = false;
        
        auto mbAppData{resize_swapchain(std::move(appData))};
        
        if(!mbAppData)
            return mbAppData;
    
        appData = std::move(*mbAppData);
    }
    else if(res != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to present", std::move(appData)});
    
    ++appData->currentResourceIndex;
    appData->currentResourceIndex %= numConcurrentResources;
    
    return std::move(appData);
}