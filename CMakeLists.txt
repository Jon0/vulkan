cmake_minimum_required(VERSION 2.8)


set(PROJECT_NAME "vulkan")
project("${PROJECT_NAME}")


# build modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/")


#include src directory
include_directories("${PROJECT_SOURCE_DIR}/")


#build executable
add_subdirectory(src)
