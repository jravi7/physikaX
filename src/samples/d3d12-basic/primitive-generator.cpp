#pragma once
#include "primitive-generator.h"

#include <DirectXColors.h>

#include <cmath>   //fabs
#include <limits>  //numeric_limits

#include "vertex-data.h"

namespace physika {
namespace primitive_gen {

using namespace DirectX;

void CreateEquilateralTriangle(float const side, VertexData* vertices, uint32_t* vertexCount,
                               uint32_t* indices, uint32_t* indexCount)
{
    if (std::abs(side - 0.0f) < std::numeric_limits<float>::epsilon()) {
        *vertexCount = 0;
        *indexCount  = 0;
        return;
    }
    if (!vertexCount || !indexCount) {
        return;
    }

    if (!vertices || !indices) {
        *vertexCount = 3;
        *indexCount  = 3;
        return;
    }

    vertices[0] = { XMFLOAT3(side * 0.5f, -side * 0.5f, 0.0f), XMFLOAT4(Colors::Blue) };
    vertices[1] = { XMFLOAT3(0, side * 0.5f, 0.0f), XMFLOAT4(Colors::Green) };
    vertices[2] = { XMFLOAT3(-side * 0.5f, -side * 0.5f, 0.0f), XMFLOAT4(Colors::Red) };

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
}

void CreateCube(float const side, VertexData* vertices, uint32_t* vertexCount, uint32_t* indices,
                uint32_t* indexCount)
{
    if (std::abs(side - 0.0f) < std::numeric_limits<float>::epsilon()) {
        *vertexCount = 0;
        *indexCount  = 0;
        return;
    }
    if (!vertexCount || !indexCount) {
        return;
    }

    if (!vertices || !indices) {
        *vertexCount = 8;
        *indexCount  = 36;
        return;
    }

    vertices[0] = { XMFLOAT3(side * 0.5f, side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) };
    vertices[1] = { XMFLOAT3(-side * 0.5f, side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) };
    vertices[2] = { XMFLOAT3(-side * 0.5f, -side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) };
    vertices[3] = { XMFLOAT3(side * 0.5f, -side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) };
    vertices[4] = { XMFLOAT3(side * 0.5f, side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) };
    vertices[5] = { XMFLOAT3(-side * 0.5f, side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) };
    vertices[6] = { XMFLOAT3(-side * 0.5f, -side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) };
    vertices[7] = { XMFLOAT3(side * 0.5f, -side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) };

    // add face
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 2;
    indices[5] = 3;

    indices[6]  = 4;
    indices[7]  = 5;
    indices[8]  = 1;
    indices[9]  = 4;
    indices[10] = 1;
    indices[11] = 0;

    indices[12] = 4;
    indices[13] = 0;
    indices[14] = 3;
    indices[15] = 4;
    indices[16] = 3;
    indices[17] = 7;

    indices[18] = 5;
    indices[19] = 4;
    indices[20] = 7;
    indices[21] = 5;
    indices[22] = 7;
    indices[23] = 6;

    indices[24] = 1;
    indices[25] = 5;
    indices[26] = 6;
    indices[27] = 1;
    indices[28] = 6;
    indices[29] = 2;

    indices[30] = 3;
    indices[31] = 2;
    indices[32] = 6;
    indices[33] = 3;
    indices[34] = 6;
    indices[35] = 7;
}

}  // namespace primitive_gen
}  // namespace physika