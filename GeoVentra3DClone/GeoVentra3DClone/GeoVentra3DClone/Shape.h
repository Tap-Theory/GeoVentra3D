#pragma once
#include <glm/glm.hpp>

enum class ShapeType {
    Cube
};

struct PlacedShape {
    ShapeType type = ShapeType::Cube;
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f };
    glm::vec3 scale{ 1.0f };
    glm::vec3 color{ 0.8f, 0.3f, 0.3f };
    bool selected = false;
};