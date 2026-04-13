#pragma once

#include "se_window.h"
#include "se_device.h"
#include "se_swap_chain.h"

#include <cassert>
#include <memory>
#include <vector>

namespace se {

class SeRenderer {
public:
    static constexpr int WIDTH = 1400;
    static constexpr int HEIGHT = 1400;

    SeRenderer(SeWindow& window, SeDevice& device);
    ~SeRenderer();

    SeRenderer(const SeRenderer&) = delete;
    SeRenderer& operator=(const SeRenderer&) = delete;

    VkRenderPass getSwapChainRenderPass() const { return seSwapChain->getRenderPass(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {
        assert(isFrameStarted && "Can not get command buffer when frame not in progress.");
        return commandBuffers[currentFrameIndex];
    }

    int getFrameIndex() const {
        assert(isFrameStarted && "Can not get frame index when frame not in progress.");
        return currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    SeWindow& seWindow;
    SeDevice& seDevice;
    std::unique_ptr<se::SeSwapChain> seSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    int currentFrameIndex = 0;
    bool isFrameStarted = false;
};
}
