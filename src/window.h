#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "pipeline.h"


class Window {
public:
    Window(VkInstance &instance, int width, int height);
    ~Window();

    VkSurfaceKHR &getSurface();
    bool shouldClose();
    void pollEvents();

    std::pair<int, int> getWindowSize();

private:
    VkInstance &instance;
    GLFWwindow *window;
    VkSurfaceKHR surface;

};
