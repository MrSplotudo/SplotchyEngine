#pragma once

#include "../engine/se_window.h"
#include "../engine/se_pipeline.h"
#include "../engine/se_device.h"
#include "../engine/se_swap_chain.h"
#include "../engine/se_model.h"

#include <memory>
#include <vector>

class FirstApp {

public:
    static constexpr int WIDTH = 1400;
    static constexpr int HEIGHT = 1400;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp &) = delete;
    FirstApp& operator= (const FirstApp &) = delete;

    void run();

private:
    void loadModel();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void freeCommandBuffers();
    void drawFrame();
    void recreateSwapChain();
    void recordCommandBuffer(int imageIndex);

    void sirepinski(std::vector<se::SeModel::Vertex>& vertices, int depth, glm::vec2 top, glm::vec2 right, glm::vec2 left);

    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    std::unique_ptr<se::SeSwapChain> seSwapChain;
    std::unique_ptr<se::SePipeline> sePipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::unique_ptr<se::SeModel> seModel;
};