#include "triangle.h"
#include "framebuffer.h"
#include <algorithm>
#include <cmath>

void drawLine(int x0, int y0, int x1, int y1, const Color& color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);
    int error = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        if (steep) setPixel(y, x, color);
        else setPixel(x, y, color);

        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

void drawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const Color& color) {
    drawLine((int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y, color);
    drawLine((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, color);
    drawLine((int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y, color);
}

void fillTriangle(const glm::vec3& v0_in, const glm::vec3& v1_in, const glm::vec3& v2_in, const Color& color) {
    glm::vec3 v0 = v0_in, v1 = v1_in, v2 = v2_in;
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);

    if (v2.y == v0.y) return;

    float dx1 = (v1.y - v0.y > 0) ? (v1.x - v0.x) / (v1.y - v0.y) : 0;
    float dx2 = (v2.y - v0.y > 0) ? (v2.x - v0.x) / (v2.y - v0.y) : 0;
    float dx3 = (v2.y - v1.y > 0) ? (v2.x - v1.x) / (v2.y - v1.y) : 0;

    float sx = v0.x;
    float ex = v0.x;

    for (int y = (int)v0.y; y <= (int)v1.y; y++) {
        for (int x = (int)sx; x <= (int)ex; x++) setPixel(x, y, color);
        sx += dx1;
        ex += dx2;
    }

    sx = v1.x;
    ex = v0.x + dx2 * (v1.y - v0.y);

    for (int y = (int)v1.y; y <= (int)v2.y; y++) {
        for (int x = (int)sx; x <= (int)ex; x++) setPixel(x, y, color);
        sx += dx3;
        ex += dx2;
    }
}