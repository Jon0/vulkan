#include <iostream>

#include "window.h"

static void glfwError(int id, const char* description) {
    std::cout << "Error: " << description << std::endl;
}


Window::Window(VkInstance &instance) {
    glfwSetErrorCallback(&glfwError);
    if (!glfwInit()) {
        std::cout << "Initialization failed" << std::endl;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    VkResult err = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (err)
    {
        std::cout << "Failed to create surface" << std::endl;
    }
}


Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}


VkSurfaceKHR Window::getSurface() {
    return surface;
}


bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}


void Window::pollEvents() {
    glfwPollEvents();
}
