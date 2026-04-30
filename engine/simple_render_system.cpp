#include "simple_render_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <iostream>
#include <array>

namespace se {
struct SimplePushConstantData {
    glm::mat4 transform{1.f};
    glm::mat4 modelMatrix{1.f};
};

SimpleRenderSystem::SimpleRenderSystem(SeDevice& device, VkRenderPass renderPass) : seDevice{device} {
    createPipelineLayout();
    createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
    vkDestroyPipelineLayout(seDevice.device(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout() {
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT  | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(seDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");

    PipelineConfigInfo pipelineConfig{};
    SePipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = renderPass;
    pipelineConfig.pipelineLayout = pipelineLayout;
    sePipeline = std::make_unique<SePipeline>(
        seDevice,
        "../shaders/simple_shader.vert.spv",
        "../shaders/simple_shader.frag.spv",
        pipelineConfig);
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<SeGameObject>& gameObjects, const SeCamera& camera) {

    sePipeline->bind(commandBuffer);

    auto projectionView = camera.getProjection() * camera.getView();

    for (auto& obj: gameObjects) {
        SimplePushConstantData push{};
        auto modelMatrix = obj.transform.mat4();
        push.transform = projectionView * obj.transform.mat4();
        push.modelMatrix = modelMatrix;

        vkCmdPushConstants(
            commandBuffer,
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);
        obj.model->bind(commandBuffer);
        obj.model->draw(commandBuffer);
    }
}
}