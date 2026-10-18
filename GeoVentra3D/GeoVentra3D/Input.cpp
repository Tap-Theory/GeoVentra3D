#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"

static glm::vec3 GetRayFromMouse(double mouseX, double mouseY, int screenWidth, int screenHeight,
    const glm::mat4& projection, const glm::mat4& view)
{
    float x = (2.0f * (float)mouseX) / (float)screenWidth - 1.0f;
    float y = 1.0f - (2.0f * (float)mouseY) / (float)screenHeight;
    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projection) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    return glm::normalize(glm::vec3(glm::inverse(view) * rayEye));
}

static bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
    const glm::vec3& boxMin, const glm::vec3& boxMax, float& outT)
{
    float t1 = (boxMin.x - rayOrigin.x) / rayDir.x;
    float t2 = (boxMax.x - rayOrigin.x) / rayDir.x;
    float t3 = (boxMin.y - rayOrigin.y) / rayDir.y;
    float t4 = (boxMax.y - rayOrigin.y) / rayDir.y;
    float t5 = (boxMin.z - rayOrigin.z) / rayDir.z;
    float t6 = (boxMax.z - rayOrigin.z) / rayDir.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    if (tmax < 0.0f || tmin > tmax) return false;
    outT = tmin;
    return true;
}

static void DuplicateSelected(InputState& s) {
    if (*s.selectedShapeIndex >= 0 && *s.selectedShapeIndex < (int)s.shapes->size()) {
        PlacedShape copy = (*s.shapes)[*s.selectedShapeIndex];
        copy.position += glm::vec3(0.6f, 0.0f, 0.6f);
        s.shapes->push_back(copy);
        *s.selectedShapeIndex = (int)s.shapes->size() - 1;
    }
}

static void CopySelected(InputState& s) {
    if (*s.selectedShapeIndex >= 0 && *s.selectedShapeIndex < (int)s.shapes->size()) {
        s.clipboardShape = (*s.shapes)[*s.selectedShapeIndex];
        s.hasClipboard = true;
    }
}

static void PasteObject(InputState& s) {
    if (s.hasClipboard) {
        PlacedShape pasted = s.clipboardShape;
        pasted.position += glm::vec3(0.6f, 0.0f, 0.6f);
        s.shapes->push_back(pasted);
        *s.selectedShapeIndex = (int)s.shapes->size() - 1;
        s.clipboardShape.position = pasted.position;
    }
}

static void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

    auto* s = static_cast<InputState*>(glfwGetWindowUserPointer(window));
    if (ImGui::GetIO().WantCaptureMouse) return;

    if (!s->middleMousePressed && !s->rightMousePressed) {
        s->firstMouse = true;
        return;
    }

    if (s->firstMouse) {
        s->lastX = (float)xpos;
        s->lastY = (float)ypos;
        s->firstMouse = false;
    }

    float xoffset = (float)xpos - s->lastX;
    float yoffset = s->lastY - (float)ypos;
    s->lastX = (float)xpos;
    s->lastY = (float)ypos;

    if (s->middleMousePressed) {
        if (s->shiftPressed) s->camera->Pan(xoffset, yoffset);
        else                 s->camera->Orbit(xoffset, yoffset);
    }
    else if (s->rightMousePressed) {
        s->camera->Orbit(xoffset, yoffset);
    }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    auto* s = static_cast<InputState*>(glfwGetWindowUserPointer(window));
    if (ImGui::GetIO().WantCaptureMouse) return;
    s->camera->Zoom((float)yoffset);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    auto* s = static_cast<InputState*>(glfwGetWindowUserPointer(window));
    if (ImGui::GetIO().WantCaptureMouse) return;

    s->shiftPressed = (mods & GLFW_MOD_SHIFT);

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        s->middleMousePressed = (action == GLFW_PRESS);
        if (s->middleMousePressed) s->firstMouse = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        s->rightMousePressed = (action == GLFW_PRESS);
        if (s->rightMousePressed) s->firstMouse = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0) return;

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
        glm::mat4 view = s->camera->GetViewMatrix();
        glm::vec3 rayOrigin = s->camera->GetPosition();
        glm::vec3 rayDir = GetRayFromMouse(xpos, ypos, width, height, projection, view);

        int closest = -1;
        float closestT = 1e9f;

        for (int i = 0; i < (int)s->shapes->size(); i++) {
            const auto& sh = (*s->shapes)[i];
            glm::vec3 minB = sh.position - sh.scale * 0.5f;
            glm::vec3 maxB = sh.position + sh.scale * 0.5f;
            float t;
            if (RayIntersectsAABB(rayOrigin, rayDir, minB, maxB, t) && t < closestT) {
                closestT = t;
                closest = i;
            }
        }
        *s->selectedShapeIndex = closest;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    auto* s = static_cast<InputState*>(glfwGetWindowUserPointer(window));
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    if (action == GLFW_PRESS) {
        bool ctrl = (mods & GLFW_MOD_CONTROL);

        if (ctrl && key == GLFW_KEY_D)      DuplicateSelected(*s);
        else if (ctrl && key == GLFW_KEY_C) CopySelected(*s);
        else if (ctrl && key == GLFW_KEY_V) PasteObject(*s);
        else if (key == GLFW_KEY_DELETE) {
            if (*s->selectedShapeIndex >= 0 && *s->selectedShapeIndex < (int)s->shapes->size()) {
                s->shapes->erase(s->shapes->begin() + *s->selectedShapeIndex);
                *s->selectedShapeIndex = -1;
            }
        }
        else if (key == GLFW_KEY_F) {
            if (*s->selectedShapeIndex >= 0 && *s->selectedShapeIndex < (int)s->shapes->size()) {
                const auto& sh = (*s->shapes)[*s->selectedShapeIndex];
                float maxScale = std::max({ sh.scale.x, sh.scale.y, sh.scale.z });
                s->camera->FocusOn(sh.position, maxScale * 4.5f + 4.0f);
            }
        }
    }
}

static void char_callback(GLFWwindow* window, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(window, c);
}

void SetupInputCallbacks(GLFWwindow* window, InputState* state) {
    glfwSetWindowUserPointer(window, state);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, char_callback);
}

void ProcessInput(GLFWwindow* window, InputState& state) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float current = (float)glfwGetTime();
    state.deltaTime = current - state.lastFrame;
    state.lastFrame = current;

    if (ImGui::GetIO().WantCaptureKeyboard) return;

    float speed = 6.5f * state.deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) state.camera->MoveForward(speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) state.camera->MoveForward(-speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) state.camera->MoveRight(-speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS &&
            glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) != GLFW_PRESS)
            state.camera->MoveRight(speed);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) state.camera->MoveHeight(speed);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) state.camera->MoveHeight(-speed);
}