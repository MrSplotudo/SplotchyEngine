#pragma once

#include "se_camera.h"
#include "se_pipeline.h"
#include "se_device.h"
#include "se_game_object.h"

#include <memory>
#include <vector>

namespace se {
class Simple2dRenderSystem {
public:
    static constexpr int WIDTH = 1000;
    static constexpr int HEIGHT = 800;

    Simple2dRenderSystem(SeDevice& device, VkRenderPass renderPass);
    ~Simple2dRenderSystem();

    Simple2dRenderSystem(const Simple2dRenderSystem &) = delete;
    Simple2dRenderSystem& operator= (const Simple2dRenderSystem &) = delete;

    void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<SeGameObject>& gameObjects, const SeCamera& camera);
private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);


    SeDevice& seDevice;
    std::unique_ptr<SePipeline> sePipeline;
    VkPipelineLayout pipelineLayout;
};
}