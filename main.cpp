#include <SDL2/SDL.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "color.h"
#include "framebuffer.h"
#include "triangle.h"
#include "objloader.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Color currentColor;
Mesh model;
bool filled = false;

void init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Error al inicializar SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
    window = SDL_CreateWindow("Software Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void setColor(const Color& color) {
    currentColor = color;
}

void render(float angle) {
    float scale = 20.0f;
    glm::vec3 offset(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));

    for (auto& face : model.faces) {
        if (face.size() < 3) continue;

        glm::vec4 v0 = rotation * glm::vec4(model.vertices[face[0]], 1.0f);
        glm::vec4 v1 = rotation * glm::vec4(model.vertices[face[1]], 1.0f);
        glm::vec4 v2 = rotation * glm::vec4(model.vertices[face[2]], 1.0f);

        glm::vec3 p0 = glm::vec3(v0.x, -v0.y, v0.z) * scale + offset;
        glm::vec3 p1 = glm::vec3(v1.x, -v1.y, v1.z) * scale + offset;
        glm::vec3 p2 = glm::vec3(v2.x, -v2.y, v2.z) * scale + offset;


        if (filled) fillTriangle(p0, p1, p2, currentColor);
        else drawTriangle(p0, p1, p2, currentColor);
    }
}

int main(int argc, char* argv[]) {
    init();
    model = loadOBJ("assets/nave_espacial.obj");
    setColor(Color(255, 255, 0));

    bool running = true;
    SDL_Event event;
    float angle = 0.0f;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
                filled = !filled;
        }

        clear(Color(0, 0, 0));
        render(angle);
        renderBuffer(renderer);
        angle += 0.5f; // rotación suave
        SDL_Delay(16); // ~60 fps
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
