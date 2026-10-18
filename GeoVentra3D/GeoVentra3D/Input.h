#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include "Camera.h"
#include "Shape.h"

struct InputState {
    Camera* camera = nullptr;
    std::vector<PlacedShape>* shapes = nullptr;
    int* selectedShapeIndex = nullptr;

    PlacedShape clipboardShape;
    bool hasClipboard = false;

    bool firstMouse = true;
    float lastX = 640.0f, lastY = 400.0f;
    bool rightMousePressed = false;
    bool middleMousePressed = false;
    bool shiftPressed = false;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
};

// Call once after creating the window
void SetupInputCallbacks(GLFWwindow* window, InputState* state);

// Call every frame
void ProcessInput(GLFWwindow* window, InputState& state);