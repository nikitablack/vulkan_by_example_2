#include "teapot_vulkan.h"
#include "Global.h"

MaybeAppDataPtr draw(AppDataPtr appData) noexcept
{
    uint32_t imageIndex{};
    if(vkAcquireNextImageKHR(appData->device, appData->swapchain, std::numeric_limits<uint64_t>::max(), appData->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to acquire swap chain image", std::move(appData)});
    
    auto const currentFence{appData->fences[appData->currentResourceIndex]};
    
    if(vkWaitForFences(appData->device, 1, &currentFence, VK_TRUE, std::numeric_limits<uint64_t>::max()) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to wait for fence", std::move(appData)});
    
    if(vkResetFences(appData->device, 1, &currentFence) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to reset fences", std::move(appData)});
    
    auto const currentCommandBuffer{appData->commandBuffers[appData->currentResourceIndex]};
    
    vkResetCommandBuffer(currentCommandBuffer, 0);
    
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;
    
    if(vkBeginCommandBuffer(currentCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to begin command buffer", std::move(appData)});
    
    VkClearColorValue clearColorValue{};
    clearColorValue.float32[0] = 1.0f;
    clearColorValue.float32[1] = 0.0f;
    clearColorValue.float32[2] = 0.0f;
    clearColorValue.float32[3] = 0.0f;
    
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
    
    vkCmdBeginRenderPass(currentCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport const viewport{0.0f, 0.0f, static_cast<float>(appData->surfaceExtent.width), static_cast<float>(appData->surfaceExtent.height), 0.0f, 1.0f};
    VkRect2D const scissor{VkOffset2D{0, 0}, VkExtent2D{appData->surfaceExtent.width, appData->surfaceExtent.height}};
    
    vkCmdSetViewport(currentCommandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(currentCommandBuffer, 0, 1, &scissor);
    
    /*if(solidMode)
    {
        vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appData->solidPipeline);
    }
    else
    {
        vkCmdBindPipeline(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appData->wireframePipeline);
    }
    
    VkDeviceSize const offset{0};
    vkCmdBindVertexBuffers(currentCommandBuffer, 0, 1, &appData->vertexBuffer, &offset);
    
    vkCmdBindIndexBuffer(currentCommandBuffer, appData->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    
    vkCmdBindDescriptorSets(currentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appData->pipelineLayout, 0, 1, &appData->descriptorSets[currentCommandBuffer], 0, nullptr);
    
    vkCmdDrawIndexed(currentCommandBuffer, static_cast<uint32_t>(appData->teapotData.patches.size()), 1, 0, 0, 0);*/
    
    vkCmdEndRenderPass(currentCommandBuffer);
    
    if(vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to end command buffer", std::move(appData)});
    
    VkPipelineStageFlags const waitStage{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &appData->imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentCommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &appData->presentFinishedSemaphore;
    
    if(vkQueueSubmit(appData->graphicsQueue, 1, &submitInfo, currentFence) != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to submit draw command buffer", std::move(appData)});
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &appData->presentFinishedSemaphore;
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
            return std::move(mbAppData);
    
        appData = std::move(*mbAppData);
    }
    else if(res != VK_SUCCESS)
        return tl::make_unexpected(AppDataError{"failed to present", std::move(appData)});
    
    ++appData->currentResourceIndex;
    appData->currentResourceIndex %= numConcurrentResources;
    
    return std::move(appData);
}