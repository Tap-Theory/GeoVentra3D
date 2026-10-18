#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    float distance = 12.0f;
    float yaw = -45.0f;
    float pitch = 25.0f;
    glm::vec3 target{ 0.0f, 0.0f, 0.0f };

    float sensitivity = 0.25f;
    float zoomSpeed = 1.8f;
    float panSpeed = 0.012f;

    glm::vec3 GetFront() const {
        return glm::normalize(target - GetPosition());
    }

    glm::vec3 GetRight() const {
        return glm::normalize(glm::cross(GetFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    glm::vec3 GetUp() const {
        return glm::normalize(glm::cross(GetRight(), GetFront()));
    }

    void MoveForward(float offset) {
        target += GetFront() * offset;
    }

    void MoveRight(float offset) {
        target += GetRight() * offset;
    }

    void MoveHeight(float offset) {
        target.y += offset;
    }

    void Pan(float xoffset, float yoffset) {
        float effectiveSpeed = panSpeed * distance;
        target -= GetRight() * (xoffset * effectiveSpeed);
        target += GetUp() * (yoffset * effectiveSpeed);
    }

    void Orbit(float xoffset, float yoffset) {
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    void Zoom(float yoffset) {
        distance -= yoffset * zoomSpeed;

        if (distance < 1.5f)  distance = 1.5f;
        if (distance > 80.0f) distance = 80.0f;
    }

    void FocusOn(const glm::vec3& point, float preferredDistance = -1.0f) {
        target = point;
        if (preferredDistance > 0.0f)
            distance = preferredDistance;
        else if (distance < 6.0f)
            distance = 6.0f;
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(GetPosition(), target, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 GetPosition() const {
        glm::vec3 pos;
        pos.x = target.x + distance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        pos.y = target.y + distance * sin(glm::radians(pitch));
        pos.z = target.z + distance * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return pos;
    }
};