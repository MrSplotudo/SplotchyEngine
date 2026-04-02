#include "first_app.h"

#include <stdexcept>
#include <iostream>
#include <array>


FirstApp::FirstApp() {
    loadModel();
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

FirstApp::~FirstApp() {
    vkDestroyPipelineLayout(seDevice.device(), pipelineLayout, nullptr);
}

void FirstApp::run() {
    while (!seWindow.shouldClose()) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(seDevice.device());
}

void FirstApp::loadModel() {
    std::vector<se::SeModel::Vertex> vertices {};

    sirepinski(vertices, 8, {0.0f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f});

    seModel = std::make_unique<se::SeModel>(seDevice, vertices);
}

void FirstApp::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(seDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }
}

void FirstApp::createPipeline() {
    auto pipelineConfig = se::SePipeline::defaultPipelineConfigInfo(seSwapChain.width(), seSwapChain.height());
    pipelineConfig.renderPass = seSwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    sePipeline = std::make_unique<se::SePipeline>(
        seDevice,
        "../shaders/simple_shader.vert.spv",
        "../shaders/simple_shader.frag.spv",
        pipelineConfig);
}

void FirstApp::createCommandBuffers() {
    commandBuffers.resize(seSwapChain.imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = seDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(seDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate memory to command buffer from command pool!");
    }

    for (int i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer!");
        }

        // The render pass has the info the pipeline needs about the layout of a frame buffer(ie. width and height)
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = seSwapChain.getRenderPass();
        renderPassInfo.framebuffer = seSwapChain.getFrameBuffer(i);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = seSwapChain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.3f, 0.3f, 0.3f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        sePipeline->bind(commandBuffers[i]);
        seModel->bind(commandBuffers[i]);
        seModel->draw(commandBuffers[i]);

        vkCmdEndRenderPass(commandBuffers[i]);
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }
}

void FirstApp::drawFrame() {
    uint32_t imageIndex;
    auto result = seSwapChain.acquireNextImage(&imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Filed to acquire swap chain image!");
    }

    result = seSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }
}

void FirstApp::sirepinski(std::vector<se::SeModel::Vertex>& vertices, int depth, glm::vec2 top, glm::vec2 right, glm::vec2 left) {
    if (depth <= 0) {
        vertices.push_back({top});
        vertices.push_back({right});
        vertices.push_back({left});
    } else {
        glm::vec2 leftTop = (left + top) * 0.5f;
        glm::vec2 rightTop = (right + top) * 0.5f;
        glm::vec2 leftRight = (left + right) * 0.5f;

        sirepinski(vertices, depth - 1, top, rightTop, leftTop);
        sirepinski(vertices, depth - 1, rightTop, right, leftRight);
        sirepinski(vertices, depth - 1, leftTop, leftRight, left);
    }
}