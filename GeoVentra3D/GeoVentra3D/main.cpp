#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"

#include "Camera.h"
#include "Shader.h"
#include "Shape.h"
#include "Input.h"
#include "Mesh.h"
#include "UI.h"

// GeoVentra3D

int main() {
    srand((unsigned)time(nullptr));

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 800, "GeoVentra3D Beta", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);

    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Core objects
    Camera camera;
    std::vector<PlacedShape> shapes;
    int selectedShapeIndex = -1;
    MeshData meshes;
    UIState ui;

    // Input state
    InputState input;
    input.camera = &camera;
    input.shapes = &shapes;
    input.selectedShapeIndex = &selectedShapeIndex;
    SetupInputCallbacks(window, &input);

    // Setup geometry
    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    SetupCube(meshes);
    SetupGrid(meshes, 10, 1.0f);   // Grid size and thickness
    SetupAxes(meshes);

    // Starter cube
    PlacedShape starter;
    starter.position = { 0.0f, 0.5f, 0.0f };
    starter.color = { 1.0f, 1.0f, 1.0f };
    shapes.push_back(starter);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        ProcessInput(window, input);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawUI(camera, shapes, selectedShapeIndex, ui);

        // Render
        glClearColor(ui.clearColor.r, ui.clearColor.g, ui.clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width > 0 && height > 0) {
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();

            shader.use();
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);
            shader.setVec3("lightPos", { 8.0f, 12.0f, 8.0f });
            shader.setVec3("viewPos", camera.GetPosition());

            DrawGrid(shader, meshes, ui.gridColor);
            DrawAxes(shader, meshes);

            for (int i = 0; i < (int)shapes.size(); i++)
                DrawCube(shader, meshes, shapes[i], i == selectedShapeIndex);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    CleanupMeshes(meshes);
    glfwTerminate();
    return 0;
}