#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include "Camera.h"
#include "Shape.h"

struct UIState {
    glm::vec3 clearColor{ 0.33f, 0.33f, 0.33f };
    glm::vec3 gridColor{ 0.7f, 0.7f, 0.7f };
    bool autoGridColor = true;
};

void DrawUI(Camera& camera, std::vector<PlacedShape>& shapes, int& selectedShapeIndex, UIState& ui);