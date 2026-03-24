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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}
}

