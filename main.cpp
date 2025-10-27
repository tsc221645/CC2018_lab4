#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "color.h"
#include "framebuffer.h"
#include "triangle.h"
#include "objloader.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Mesh model;


std::vector<float> zBuffer;

glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, -0.7f, 0.5f));

struct Face {
    glm::vec3 p0, p1, p2;  
    glm::vec3 n0, n1, n2;  
    Color baseColor;
};

void init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    window = SDL_CreateWindow("Renderer 3D con Z-Buffer y Sombras",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    

    zBuffer.resize(SCREEN_WIDTH * SCREEN_HEIGHT);
}

void clearZBuffer() {
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());
}


void fillTriangleWithShading(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                              const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2,
                              const Color& baseColor) {

    float minX = std::max(0.0f, std::min({v0.x, v1.x, v2.x}));
    float maxX = std::min((float)SCREEN_WIDTH - 1, std::max({v0.x, v1.x, v2.x}));
    float minY = std::max(0.0f, std::min({v0.y, v1.y, v2.y}));
    float maxY = std::min((float)SCREEN_HEIGHT - 1, std::max({v0.y, v1.y, v2.y}));

    for (int y = (int)minY; y <= (int)maxY; y++) {
        for (int x = (int)minX; x <= (int)maxX; x++) {
            glm::vec2 p(x + 0.5f, y + 0.5f);
            

            glm::vec2 v0v1 = glm::vec2(v1.x - v0.x, v1.y - v0.y);
            glm::vec2 v0v2 = glm::vec2(v2.x - v0.x, v2.y - v0.y);
            glm::vec2 v0p = glm::vec2(p.x - v0.x, p.y - v0.y);
            
            float denom = v0v1.x * v0v2.y - v0v2.x * v0v1.y;
            if (std::abs(denom) < 1e-6f) continue;
            
            float w1 = (v0p.x * v0v2.y - v0v2.x * v0p.y) / denom;
            float w2 = (v0v1.x * v0p.y - v0p.x * v0v1.y) / denom;
            float w0 = 1.0f - w1 - w2;
            

            if (w0 < 0 || w1 < 0 || w2 < 0) continue;
            

            float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
            

            int bufferIndex = y * SCREEN_WIDTH + x;
            if (z >= zBuffer[bufferIndex]) continue;
            zBuffer[bufferIndex] = z;
            

            glm::vec3 normal = glm::normalize(w0 * n0 + w1 * n1 + w2 * n2);
            

            float diffuse = glm::max(glm::dot(normal, -lightDir), 0.0f);
            

            float ambient = 0.2f;
            

            float intensity = glm::clamp(ambient + diffuse * 0.8f, 0.0f, 1.0f);
            

            uint8_t r = (uint8_t)(baseColor.r * intensity);
            uint8_t g = (uint8_t)(baseColor.g * intensity);
            uint8_t b = (uint8_t)(baseColor.b * intensity);
            
            setPixel(x, y, Color(r, g, b));
        }
    }
}

void render(float angle) {
    clear(Color(20, 20, 40));
    clearZBuffer();

    glm::vec3 minV(1e9f), maxV(-1e9f);
    for (auto& v : model.vertices) {
        minV = glm::min(minV, v);
        maxV = glm::max(maxV, v);
    }
    glm::vec3 center = (minV + maxV) * 0.5f;
    glm::vec3 diff = maxV - minV;
    float maxSize = std::max({diff.x, diff.y, diff.z});
    float scale = 2.0f / maxSize;


    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::rotate(modelMat, glm::radians(angle), glm::vec3(0, 1, 0));
    modelMat = glm::scale(modelMat, glm::vec3(scale));

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 0.5f, 3.5f),
                                    glm::vec3(0, 0, 0),
                                    glm::vec3(0, 1, 0));

    glm::mat4 projMat = glm::perspective(glm::radians(60.0f),
                                         (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
                                         0.1f, 100.0f);

    glm::mat4 mv = viewMat * modelMat;
    glm::mat4 mvp = projMat * mv;
    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(mv)));

    std::vector<Face> faces;
    faces.reserve(model.faces.size());

    std::vector<glm::vec3> vertexNormals(model.vertices.size(), glm::vec3(0.0f));
    std::vector<int> normalCounts(model.vertices.size(), 0);

    for (auto& face : model.faces) {
        if (face.size() < 3) continue;

        glm::vec3 m0 = model.vertices[face[0]] - center;
        glm::vec3 m1 = model.vertices[face[1]] - center;
        glm::vec3 m2 = model.vertices[face[2]] - center;

        glm::vec3 normal = glm::normalize(glm::cross(m1 - m0, m2 - m0));
        
        vertexNormals[face[0]] += normal;
        vertexNormals[face[1]] += normal;
        vertexNormals[face[2]] += normal;
        normalCounts[face[0]]++;
        normalCounts[face[1]]++;
        normalCounts[face[2]]++;
    }

    for (size_t i = 0; i < vertexNormals.size(); i++) {
        if (normalCounts[i] > 0) {
            vertexNormals[i] /= (float)normalCounts[i];
            vertexNormals[i] = glm::normalize(vertexNormals[i]);
        }
    }

    for (auto& face : model.faces) {
        if (face.size() < 3) continue;

        glm::vec3 m0 = model.vertices[face[0]] - center;
        glm::vec3 m1 = model.vertices[face[1]] - center;
        glm::vec3 m2 = model.vertices[face[2]] - center;

        glm::vec3 v0 = glm::vec3(mv * glm::vec4(m0, 1.0f));
        glm::vec3 v1 = glm::vec3(mv * glm::vec4(m1, 1.0f));
        glm::vec3 v2 = glm::vec3(mv * glm::vec4(m2, 1.0f));

        glm::vec3 n0 = glm::normalize(normalMat * vertexNormals[face[0]]);
        glm::vec3 n1 = glm::normalize(normalMat * vertexNormals[face[1]]);
        glm::vec3 n2 = glm::normalize(normalMat * vertexNormals[face[2]]);


        glm::vec4 c0 = mvp * glm::vec4(m0, 1.0f);
        glm::vec4 c1 = mvp * glm::vec4(m1, 1.0f);
        glm::vec4 c2 = mvp * glm::vec4(m2, 1.0f);

        c0 /= c0.w; c1 /= c1.w; c2 /= c2.w;

        auto toScreen = [&](glm::vec4 v) {
            return glm::vec3((v.x * 0.5f + 0.5f) * SCREEN_WIDTH,
                             (1.0f - (v.y * 0.5f + 0.5f)) * SCREEN_HEIGHT,
                             v.z);
        };

        Face f;
        f.p0 = toScreen(c0);
        f.p1 = toScreen(c1);
        f.p2 = toScreen(c2);
        f.n0 = n0;
        f.n1 = n1;
        f.n2 = n2;
        f.baseColor = Color(180, 180, 60); 
        
        faces.push_back(f);
    }


    for (auto& f : faces) {
        fillTriangleWithShading(f.p0, f.p1, f.p2, f.n0, f.n1, f.n2, f.baseColor);
    }

    renderBuffer(renderer);
}

int main(int argc, char* argv[]) {
    init();
    model = loadOBJ("assets/nave_espacial.obj");

    bool running = true;
    SDL_Event e;
    float angle = 0.0f;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
        }

        render(angle);
        angle += 0.8f;
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}