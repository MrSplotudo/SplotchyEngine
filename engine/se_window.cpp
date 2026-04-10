#include "se_window.h"

#include <stdexcept>

namespace se {

SeWindow::SeWindow(int widthIn, int heightIn, std::string titleIn) : width{widthIn}, height {heightIn}, title {titleIn} {
    initWindow();
}

SeWindow::~SeWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void SeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

void SeWindow::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void SeWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto seWindow = reinterpret_cast<SeWindow*>(glfwGetWindowUserPointer(window));
    seWindow->framebufferResized = true;
    seWindow->width = width;
    seWindow->height = height;
}
}

