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

	int w, h;
	glfwGetWindowSize(window, &w, &h);

	std::cout << "Window size" << w << ", " << h << std::endl;
}


std::pair<int, int> Window::getWindowSize() {
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	return std::make_pair(w, h);
}
