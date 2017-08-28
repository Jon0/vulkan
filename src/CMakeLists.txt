
# executable names
set(VULKAN_NAME "vkrun")


# compile configuration
add_definitions(-std=c++17)


find_package(PkgConfig REQUIRED)
find_package(Vulkan REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLM REQUIRED glm)

# set additional include paths
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})


file(GLOB VULKAN_FILES
	"main.cpp"
	"device.cpp"
	"pipeline.cpp"
	"renderpass.cpp"
	"surface.cpp"
	"swapchain.cpp"
	"window.cpp"
)


add_executable(
	${VULKAN_NAME}
	${VULKAN_FILES}
)


target_link_libraries(
	${VULKAN_NAME}
	${Vulkan_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLM_LIBRARIES}
)
