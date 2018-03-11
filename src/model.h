#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "vertex.h"


class ObjFile {
public:
    ObjFile(const std::string &filepath);
    GeometryBuilder &getBuilder();


private:
    glm::vec4 *vertObjIndex(int index);
    glm::vec3 *texObjIndex(int index);
    glm::vec3 *normalObjIndex(int index);

    std::vector<std::string> split(const std::string &str, char delim);
    void parseLine(const std::string &line);
    glm::vec3 parseVec3(const std::vector<std::string> components);
    glm::vec4 parseVec4(const std::vector<std::string> components);
    std::vector<Vertex> parseFace(const std::vector<std::string> components);

    GeometryBuilder builder;
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> texCoords;
    std::vector<glm::vec3> normals;

};
