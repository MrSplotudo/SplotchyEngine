#pragma once

#include "../engine/se_window.h"
#include "../engine/se_pipeline.h"
#include "../engine/se_device.h"
#include "../engine/se_swap_chain.h"

#include <memory>
#include <vector>

class FirstApp {

public:
    static constexpr int WIDTH = 600;
    static constexpr int HEIGHT = 600;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp &) = delete;
    FirstApp &operator= (const FirstApp &) = delete;

    void run();

private:
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    se::SeSwapChain seSwapChain{seDevice, seWindow.getExtent()};
    std::unique_ptr<se::SePipeline> sePipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};