#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "color.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

extern std::vector<std::vector<Color>> framebuffer;

void clear(const Color& clearColor = Color(0, 0, 0));
void setPixel(int x, int y, const Color& color);
void renderBuffer(SDL_Renderer* renderer);
