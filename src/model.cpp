#include <glm/ext.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "model.h"

ObjFile::ObjFile(const std::string &filepath) {
    std::ifstream objFile(filepath);
    std::string line;
    while (std::getline(objFile, line)) {
        parseLine(line);
    }
}


GeometryBuilder &ObjFile::getBuilder() {
    return builder;
}


glm::vec4 *ObjFile::vertObjIndex(int index) {
    if (index) {
        int realIndex = (index - 1) % vertices.size();
        return &vertices[realIndex];
    }
    return nullptr;
}


glm::vec3 *ObjFile::texObjIndex(int index) {
    if (index) {
        int realIndex = (index - 1) % texCoords.size();
        return &texCoords[realIndex];
    }
    return nullptr;
}


glm::vec3 *ObjFile::normalObjIndex(int index) {
    if (index) {
        int realIndex = (index - 1) % normals.size();
        return &normals[realIndex];
    }
    return nullptr;
}


std::vector<std::string> ObjFile::split(const std::string &str, char delim) {
    std::stringstream ss(str);
    std::string item;
    std::vector<std::string> items;
    while (std::getline(ss, item, delim)) {
        items.emplace_back(item);
    }
    return items;
}


void ObjFile::parseLine(const std::string &line) {
    std::vector<std::string> items = split(line, ' ');
    if (items.empty()) {
        return;
    }
    const std::string &first = items[0];
    std::vector<std::string> components(items.begin () + 1, items.end());
    if (first == "v") {
        vertices.push_back(parseVec4(components));
    }
    else if (first == "vt") {
        texCoords.push_back(parseVec3(components));
    }
    else if (first == "vn") {
        normals.push_back(parseVec3(components));
    }
    else if (first == "f") {
        builder.addPolygon(parseFace(components));
    }
}


glm::vec3 ObjFile::parseVec3(const std::vector<std::string> components) {
    glm::vec3 data;
    size_t end = std::min<size_t>(3, components.size());
    for (size_t i = 0; i < end; ++i) {
        data[i] = std::stof(components[i]);
    }
    for (size_t i = end; i < 3; ++i) {
        data[i] = 1.0f;
    }
    return data;
}


glm::vec4 ObjFile::parseVec4(const std::vector<std::string> components) {
    glm::vec4 data;
    size_t end = std::min<size_t>(4, components.size());
    for (size_t i = 0; i < end; ++i) {
        data[i] = std::stof(components[i]);
    }
    for (size_t i = end; i < 4; ++i) {
        data[i] = 1.0f;
    }
    return data;
}


std::vector<Vertex> ObjFile::parseFace(const std::vector<std::string> components) {
    std::vector<Vertex> result;
    for (auto &str : components) {
        Vertex vert;
        std::vector<std::string> items = split(str, '/');
        size_t count = items.size();
        glm::vec4 *p = count > 0 && !items[0].empty() ? vertObjIndex(std::stoi(items[0])) : nullptr;
        glm::vec3 *t = count > 1 && !items[1].empty() ? texObjIndex(std::stoi(items[1])) : nullptr;
        glm::vec3 *n = count > 2 && !items[2].empty() ? normalObjIndex(std::stoi(items[2])) : nullptr;


        if (p) {
            vert.pos = *p;
        }
        else {
            vert.pos = glm::vec4(1.0f);
        }
        if (t) {
            vert.texCoord = *t;
        }
        else {
            vert.texCoord = glm::vec3(1.0f);
        }
        if (n) {
            vert.normal = *n;
        }
        else {
            vert.normal = glm::vec3(1.0f);
        }
        result.push_back(vert);
    }
    return result;
}
