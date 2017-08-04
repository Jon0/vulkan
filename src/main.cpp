#include <vector>

#include "window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>




int main() {
    Pipeline pipeline;
    Window window(pipeline.getInstance());

    //vertShaderModule = device.loadShaderModule("shaders/vert.spv");
    //fragShaderModule = device.loadShaderModule("shaders/frag.spv");

    while(!window.shouldClose()) {
        window.pollEvents();
    }


    return 0;
}
