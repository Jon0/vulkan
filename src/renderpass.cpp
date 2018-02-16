#include <iostream>

#include "renderpass.h"


RenderPass::RenderPass(VkPhysicalDevice &physicalDevice, VkDevice &device, const VkFormat &swapChainImageFormat)
    :
    device {device},
    vertexData {physicalDevice, device} {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
    if (result != VK_SUCCESS) {
        std::cout << "Failed to create render pass" << std::endl;
    }
}


RenderPass::~RenderPass() {
    vkWaitForFences(device, fences.size(), fences.data(), true, std::numeric_limits<uint64_t>::max());
    for (size_t i = 0; i < fences.size(); ++i) {
        vkDestroyFence(device, fences[i], nullptr);
    }
    vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
}


VkRenderPass &RenderPass::getVulkanRenderPass() {
    return renderPass;
}


void RenderPass::initCommandPool(Device &device, Pipeline &pipeline, SwapChain &swapChain) {
    device.createCommandPool(commandPool);
    allocateCommandBuffers(commandBuffers, device.getVulkanDevice(), swapChain.getFramebufferSize());

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        beginInfo.pInheritanceInfo = nullptr; // Optional

        vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = swapChain.getFrameBuffer(i);
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = swapChain.getExtent();
        VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;
        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getVulkanPipeline());
        vertexData.render(commandBuffers[i]);
        vkCmdEndRenderPass(commandBuffers[i]);
        VkResult result = vkEndCommandBuffer(commandBuffers[i]);
        if (result != VK_SUCCESS) {
            std::cout << "Failed to record command buffer" << std::endl;
        }
    }

    device.createSemaphore(imageAvailableSemaphore);
    device.createSemaphore(renderFinishedSemaphore);

    // unused?
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    //renderPassInfo.dependencyCount = 1;
    //renderPassInfo.pDependencies = &dependency;
}



void RenderPass::renderFrame(Device &device, VkSwapchainKHR &swapChain) {
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device.getVulkanDevice(), swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    vkWaitForFences(device.getVulkanDevice(), 1, &fences[imageIndex], true, std::numeric_limits<uint64_t>::max());
    vkResetFences(device.getVulkanDevice(), 1, &fences[imageIndex]);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, fences[imageIndex]) != VK_SUCCESS) {
        std::cout << "failed to submit draw command buffer!" << std::endl;
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(device.getGraphicsQueue(), &presentInfo);
    vkQueueWaitIdle(device.getGraphicsQueue());
}


void RenderPass::allocateCommandBuffers(std::vector<VkCommandBuffer> &commandBuffers, VkDevice &device, size_t bufferCount) {
    commandBuffers.resize(bufferCount);
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) bufferCount;

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
    if (result != VK_SUCCESS) {
        std::cout << "Failed to allocate command buffers" << std::endl;
    }


    fences.resize(bufferCount);
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    for (size_t i = 0; i < bufferCount; ++i) {
        vkCreateFence(device, &fenceInfo, nullptr, &fences[i]);
    }
}
