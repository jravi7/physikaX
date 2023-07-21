#pragma once

#include "common/primitive-generator.h"

#include <DirectXColors.h>

#include <cmath>   //fabs
#include <limits>  //numeric_limits

namespace physika::common {

using namespace DirectX;

MeshData CreateEquilateralTriangle(float const side)
{
    MeshData meshData;
    if (std::abs(side - 0.0f) < std::numeric_limits<float>::epsilon()) {
        return meshData;
    }
    float halfSide    = side * 0.5f;
    meshData.vertices = { { XMFLOAT3(halfSide, -halfSide, 0.0f), XMFLOAT4(Colors::Blue) },
                          { XMFLOAT3(0, halfSide, 0.0f), XMFLOAT4(Colors::Green) },
                          { XMFLOAT3(-halfSide, -halfSide, 0.0f), XMFLOAT4(Colors::Red) } };

    meshData.indices = { 0, 1, 2 };
    return meshData;
}
MeshData CreateCube(float const side)
{
    MeshData meshData;
    if (std::abs(side - 0.0f) < std::numeric_limits<float>::epsilon()) {
        return meshData;
    }
    float halfSide    = side * 0.5f;
    meshData.vertices = {
        { XMFLOAT3(halfSide, halfSide, halfSide), XMFLOAT4(Colors::Blue) },
        { XMFLOAT3(-halfSide, halfSide, halfSide), XMFLOAT4(Colors::Green) },
        { XMFLOAT3(-halfSide, -halfSide, halfSide), XMFLOAT4(Colors::Red) },
        { XMFLOAT3(halfSide, -halfSide, halfSide), XMFLOAT4(Colors::Yellow) },
        { XMFLOAT3(halfSide, halfSide, -halfSide), XMFLOAT4(Colors::ForestGreen) },
        { XMFLOAT3(-halfSide, halfSide, -halfSide), XMFLOAT4(Colors::LightGoldenrodYellow) },
        { XMFLOAT3(-halfSide, -halfSide, -halfSide), XMFLOAT4(Colors::Cyan) },
        { XMFLOAT3(halfSide, -halfSide, -halfSide), XMFLOAT4(Colors::Magenta) }
    };

    // add face
    /*
     * g +------+ b
     *   |      |
     *   |      |
     * r +------+ y
     */
    // clang-format off
    meshData.indices = {
        0, 1, 2, 
        0, 2, 3,

        4, 5, 1, 
        4, 1, 0,

        4, 0, 3, 
        4, 3, 7,

        5, 4, 7, 
        5, 7, 6,

        1, 5, 6, 
        1, 6, 2,

        3, 2, 6, 
        3, 6, 7,
    };
    // clang-format on

    return meshData;
}

}  // namespace physika::common