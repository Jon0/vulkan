
# build shaders
execute_process(
    COMMAND glslangValidator -V shader.frag
    COMMAND glslangValidator -V shader.vert
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders/"
)

# copy to build directory
file(
    COPY "${PROJECT_SOURCE_DIR}/shaders/"
    DESTINATION "${PROJECT_BINARY_DIR}/shaders/"
)
