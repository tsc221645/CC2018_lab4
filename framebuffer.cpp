#include "framebuffer.h"

std::vector<std::vector<Color>> framebuffer(SCREEN_HEIGHT, std::vector<Color>(SCREEN_WIDTH, Color(0, 0, 0)));

void clear(const Color& clearColor) {
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            framebuffer[y][x] = clearColor;
}

void setPixel(int x, int y, const Color& color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
        framebuffer[y][x] = color;
}

void renderBuffer(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            Color c = framebuffer[y][x];
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);
}
