#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "pipeline.h"


class Window {
public:
    Window(VkInstance &instance);
    ~Window();

    VkSurfaceKHR &getSurface();
    bool shouldClose();
    void pollEvents();

private:
    GLFWwindow *window;
    VkSurfaceKHR surface;

};
