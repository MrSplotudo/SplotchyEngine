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
    SeWindow& operator= (const SeWindow &) = delete;

    bool shouldClose() { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
    bool wasWindowResized() { return framebufferResized; }
    void resetWindowResizedFlag() { framebufferResized = false; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void initWindow();

    int width;
    int height;
    bool framebufferResized = false;

    std::string title;
    GLFWwindow* window;
};
}

