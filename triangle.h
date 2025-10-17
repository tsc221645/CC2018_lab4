#pragma once
#include <glm/glm.hpp>
#include "color.h"

void drawLine(int x0, int y0, int x1, int y1, const Color& color);
void drawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const Color& color);
void fillTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const Color& color);
