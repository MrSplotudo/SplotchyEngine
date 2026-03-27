#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace se {

class SeWindow {
public:
    SeWindow(int widthIn, int heightIn, std::string titleIn);
    ~SeWindow();

    SeWindow(const SeWindow &) = delete;
    SeWindow &operator= (const SeWindow &) = delete;

    bool shouldClose() { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    void initWindow();

    const int width;
    const int height;

    std::string title;
    GLFWwindow* window;
};
}

