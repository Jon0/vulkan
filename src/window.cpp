#include <iostream>

#include "window.h"


Window::Window(VkInstance &instance, int width, int height)
	:
	instance {instance} {

	std::cout << "Creating GLFW window" << std::endl;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, "Vulkan window", nullptr, nullptr);
	std::cout << "Window created" << std::endl;
	VkResult err = glfwCreateWindowSurface(instance, window, NULL, &surface);
	if (err)
	{
		std::cout << "Failed to create surface" << std::endl;
	}

}


Window::~Window() {
	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(window);
}


VkSurfaceKHR &Window::getSurface() {
	return surface;
}


bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}


void Window::pollEvents() {
	glfwPollEvents();
}
