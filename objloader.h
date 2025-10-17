#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>

struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<std::vector<int>> faces;
};

Mesh loadOBJ(const std::string& path);
