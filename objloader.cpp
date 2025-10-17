#include "objloader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Mesh loadOBJ(const std::string& path) {
    Mesh mesh;
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << path << std::endl;
        return mesh;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            mesh.vertices.push_back(vertex);
        } else if (prefix == "f") {
            std::vector<int> face;
            std::string vertexData;
            while (ss >> vertexData) {
                std::stringstream vs(vertexData);
                std::string index;
                std::getline(vs, index, '/');
                face.push_back(std::stoi(index) - 1);
            }
            mesh.faces.push_back(face);
        }
    }

    file.close();
    std::cout << "Modelo cargado: " << mesh.vertices.size() << " vertices, " 
              << mesh.faces.size() << " caras." << std::endl;
    return mesh;
}
