#include <glad/glad.h>
#include "Mesh.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

void SetupCube(MeshData& m) {
    float vertices[] = {

        -0.5f,-0.5f,-0.5f,  0,0,-1,  0.5f,-0.5f,-0.5f,  0,0,-1,  0.5f,0.5f,-0.5f,  0,0,-1,
         0.5f,0.5f,-0.5f,   0,0,-1, -0.5f,0.5f,-0.5f,  0,0,-1, -0.5f,-0.5f,-0.5f,  0,0,-1,

        -0.5f,-0.5f,0.5f,   0,0,1,   0.5f,-0.5f,0.5f,   0,0,1,   0.5f,0.5f,0.5f,   0,0,1,
         0.5f,0.5f,0.5f,    0,0,1,  -0.5f,0.5f,0.5f,   0,0,1,  -0.5f,-0.5f,0.5f,   0,0,1,

        -0.5f,0.5f,0.5f,   -1,0,0,  -0.5f,0.5f,-0.5f, -1,0,0,  -0.5f,-0.5f,-0.5f,-1,0,0,
        -0.5f,-0.5f,-0.5f, -1,0,0,  -0.5f,-0.5f,0.5f, -1,0,0,  -0.5f,0.5f,0.5f,  -1,0,0,

         0.5f,0.5f,0.5f,    1,0,0,   0.5f,0.5f,-0.5f,  1,0,0,   0.5f,-0.5f,-0.5f, 1,0,0,
         0.5f,-0.5f,-0.5f,  1,0,0,   0.5f,-0.5f,0.5f,  1,0,0,   0.5f,0.5f,0.5f,   1,0,0,

        -0.5f,-0.5f,-0.5f,  0,-1,0,  0.5f,-0.5f,-0.5f, 0,-1,0,  0.5f,-0.5f,0.5f,  0,-1,0,
         0.5f,-0.5f,0.5f,   0,-1,0, -0.5f,-0.5f,0.5f,  0,-1,0, -0.5f,-0.5f,-0.5f, 0,-1,0,

        -0.5f,0.5f,-0.5f,   0,1,0,   0.5f,0.5f,-0.5f,  0,1,0,   0.5f,0.5f,0.5f,   0,1,0,
         0.5f,0.5f,0.5f,    0,1,0,  -0.5f,0.5f,0.5f,   0,1,0,  -0.5f,0.5f,-0.5f,  0,1,0
    };

    glGenVertexArrays(1, &m.cubeVAO);
    glGenBuffers(1, &m.cubeVBO);
    glBindVertexArray(m.cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void SetupGrid(MeshData& m, int size, float step) {
    std::vector<float> verts;
    for (int i = -size; i <= size; ++i) {
        // X direction
        verts.insert(verts.end(), { (float)i * step, 0, (float)-size * step, 0,1,0 });
        verts.insert(verts.end(), { (float)i * step, 0, (float)size * step, 0,1,0 });
        // Z direction
        verts.insert(verts.end(), { (float)-size * step, 0, (float)i * step, 0,1,0 });
        verts.insert(verts.end(), { (float)size * step, 0, (float)i * step, 0,1,0 });
    }
    m.gridVertexCount = (int)(verts.size() / 6);

    glGenVertexArrays(1, &m.gridVAO);
    glGenBuffers(1, &m.gridVBO);
    glBindVertexArray(m.gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.gridVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void SetupAxes(MeshData& m, float length) {
    float axes[] = {
        0,0,0, 1,0.15f,0.15f,   length,0,0, 1,0.15f,0.15f,   // X
        0,0,0, 0.2f,1,0.2f,     0,length,0, 0.2f,1,0.2f,     // Y
        0,0,0, 0.2f,0.4f,1,     0,0,length, 0.2f,0.4f,1      // Z
    };

    glGenVertexArrays(1, &m.axesVAO);
    glGenBuffers(1, &m.axesVBO);
    glBindVertexArray(m.axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.axesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void DrawGrid(const Shader& shader, const MeshData& m, const glm::vec3& gridColor) {
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setVec3("objectColor", gridColor);
    glBindVertexArray(m.gridVAO);
    glDrawArrays(GL_LINES, 0, m.gridVertexCount);
    glBindVertexArray(0);
}

void DrawAxes(const Shader& shader, const MeshData& m) {
    shader.setMat4("model", glm::mat4(1.0f));
    glBindVertexArray(m.axesVAO);
    glLineWidth(2.8f);

    shader.setVec3("objectColor", { 1.0f, 0.2f, 0.2f });
    glDrawArrays(GL_LINES, 0, 2);
    shader.setVec3("objectColor", { 0.25f, 1.0f, 0.25f });
    glDrawArrays(GL_LINES, 2, 2);
    shader.setVec3("objectColor", { 0.25f, 0.45f, 1.0f });
    glDrawArrays(GL_LINES, 4, 2);

    glLineWidth(1.0f);
    glBindVertexArray(0);
}

void DrawCube(const Shader& shader, const MeshData& m, const PlacedShape& shape, bool isSelected) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, shape.position);
    model = glm::rotate(model, glm::radians(shape.rotation.x), { 1,0,0 });
    model = glm::rotate(model, glm::radians(shape.rotation.y), { 0,1,0 });
    model = glm::rotate(model, glm::radians(shape.rotation.z), { 0,0,1 });
    model = glm::scale(model, shape.scale);

    shader.setMat4("model", model);
    glBindVertexArray(m.cubeVAO);

    shader.setVec3("objectColor", shape.color);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    if (isSelected) {
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(3.5f);
        shader.setVec3("objectColor", { 0.894f, 0.769f, 1.0f });
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glLineWidth(1.0f);
    }
}

void CleanupMeshes(MeshData& m) {
    glDeleteVertexArrays(1, &m.cubeVAO); glDeleteBuffers(1, &m.cubeVBO);
    glDeleteVertexArrays(1, &m.gridVAO); glDeleteBuffers(1, &m.gridVBO);
    glDeleteVertexArrays(1, &m.axesVAO); glDeleteBuffers(1, &m.axesVBO);
}