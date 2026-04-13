#pragma once

#include "../engine/se_pipeline.h"
#include "../engine/se_device.h"
#include "../engine/se_game_object.h"

#include <memory>
#include <vector>

namespace se {
class SimpleRenderSystem {
public:
    static constexpr int WIDTH = 1000;
    static constexpr int HEIGHT = 800;

    SimpleRenderSystem(SeDevice& device, VkRenderPass renderPass);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem& operator= (const SimpleRenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<SeGameObject>& gameObjects);
private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);


    SeDevice& seDevice;
    std::unique_ptr<SePipeline> sePipeline;
    VkPipelineLayout pipelineLayout;
};
}