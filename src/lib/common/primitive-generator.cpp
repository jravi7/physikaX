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

    meshData.indices = { 2, 1, 0 };
    return meshData;
}
MeshData CreateCube(float const side)
{
    MeshData meshData;
    if (std::abs(side - 0.0f) < std::numeric_limits<float>::epsilon()) {
        return meshData;
    }
    float halfSide    = side * 0.5f;
    meshData.vertices = { { XMFLOAT3(halfSide, halfSide, -halfSide), XMFLOAT4(Colors::Blue) },
                          { XMFLOAT3(-halfSide, halfSide, -halfSide), XMFLOAT4(Colors::Green) },
                          { XMFLOAT3(-halfSide, -halfSide, -halfSide), XMFLOAT4(Colors::Red) },
                          { XMFLOAT3(halfSide, -halfSide, -halfSide), XMFLOAT4(Colors::Yellow) },

                          { XMFLOAT3(halfSide, halfSide, halfSide), XMFLOAT4(Colors::ForestGreen) },
                          { XMFLOAT3(-halfSide, halfSide, halfSide),
                            XMFLOAT4(Colors::LightGoldenrodYellow) },
                          { XMFLOAT3(-halfSide, -halfSide, halfSide), XMFLOAT4(Colors::Cyan) },
                          { XMFLOAT3(halfSide, -halfSide, halfSide), XMFLOAT4(Colors::Magenta) } };
    /*


         y+
         ^
         |   z+
         |  /
         | /
         |/
         +---------> x+

          5------4
         /|     /|
        1------0 |
        | |    | |
        | 6----|-7
        |/     |/
        2------3
    */
    // clang-format off
    meshData.indices = {
        //front
        0, 3, 2, 
        0, 2, 1,

        //back
        5, 6, 7, 
        5, 7, 4,
        
        //right
        4, 7, 3, 
        4, 3, 0,

        //left
        1, 2, 6, 
        1, 6, 5,

        //top
        4, 0, 1, 
        4, 1, 5,

        //bottom
        3, 7, 6, 
        3, 6, 2,
    };
    // clang-format on

    return meshData;
}

}  // namespace physika::common