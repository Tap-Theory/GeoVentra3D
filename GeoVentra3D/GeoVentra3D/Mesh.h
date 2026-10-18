#pragma once
#pragma once
#include <glad/glad.h>
#include "Shader.h"
#include "Shape.h"

struct MeshData {
    unsigned int cubeVAO = 0, cubeVBO = 0;
    unsigned int gridVAO = 0, gridVBO = 0;
    unsigned int axesVAO = 0, axesVBO = 0;
    int gridVertexCount = 0;
};

void SetupCube(MeshData& m);
void SetupGrid(MeshData& m, int size = 50, float step = 1.0f);
void SetupAxes(MeshData& m, float length = 10.0f);

void DrawGrid(const Shader& shader, const MeshData& m, const glm::vec3& gridColor);
void DrawAxes(const Shader& shader, const MeshData& m);
void DrawCube(const Shader& shader, const MeshData& m, const PlacedShape& shape, bool isSelected);

void CleanupMeshes(MeshData& m);