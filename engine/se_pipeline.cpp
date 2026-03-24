#include "se_pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace se {




SePipeline::SePipeline(
    SeDevice &device,
    const std::string &vertFilepath,
    const std::string &fragFilepath,
    const PipelineConfigInfo &configInfo) : seDevice{device} {

    createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
}

PipelineConfigInfo SePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
    PipelineConfigInfo configInfo{};

    return configInfo;
}

void SePipeline::createShaderModule(const std::vector<std::string> &code, VkShaderModule*shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(seDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }
}

std::vector<char> SePipeline::readFile(const std::string& filepath) {

    std::ifstream file{filepath, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

void SePipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) {

    auto vertcode = readFile(vertFilepath);
    auto fragCode = readFile(fragFilepath);

    std::cout << "Vertex shader code size: " << vertcode.size() << std::endl;
    std::cout << "Fragment shader code size: " << fragCode.size() << std::endl;
}

}