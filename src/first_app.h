#pragma once

#include "../engine/se_window.h"
#include "../engine/se_pipeline.h"
#include "../engine/se_device.h"

class FirstApp {

public:
    static constexpr int WIDTH = 600;
    static constexpr int HEIGHT = 600;

    void run();

private:
    se::SeWindow seWindow{WIDTH, HEIGHT, "Splotchy's little window"};
    se::SeDevice seDevice{seWindow};
    se::SePipeline sePipeline{
        seDevice,
        "../shaders/simple_shader.vert.spv",
        "../shaders/simple_shader.frag.spv",
        se::SePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
    };
};