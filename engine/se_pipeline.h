#pragma once

#include "se_device.h"

#include <string>
#include <vector>

namespace se {

struct PipelineConfigInfo {};

class SePipeline {
public:
    SePipeline(
        SeDevice& device,
        const std::string& vertFilepath,
        const std::string& fragFilepath,
        const PipelineConfigInfo& configInfo);
    ~SePipeline() {}

    SePipeline(const SePipeline&) = delete;
    SePipeline& operator=(const SePipeline&) = delete;

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

private:
    static std::vector<char> readFile(const std::string& filepath);

    void createGraphicsPipeline(
        const std::string& vertFilepath,
        const std::string& fragFilepath,
        const PipelineConfigInfo& configInfo);

    void createShaderModule(const std::vector<std::string>& code, VkShaderModule* shaderModule);

    SeDevice& seDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};
}