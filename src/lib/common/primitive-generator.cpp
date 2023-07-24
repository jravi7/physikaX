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

/*

          0------1------2
         /      /      /
        3------4------5
       /      /      /
      6------7------8

*/

MeshData CreateUniformGrid(int side, int cellSize)
{
    MeshData meshData;
    int halfSide         = side / 2;
    int cellCountPerRow  = side / cellSize;
    int pointCountPerRow = cellCountPerRow + 1;
    for (int z = halfSide; z >= -halfSide; z -= cellSize) {
        for (int x = -halfSide; x <= halfSide; x += cellSize) {
            VertexData vertex = { { float(x), 0, float(z) }, XMFLOAT4(Colors::White) };
            meshData.vertices.push_back(vertex);
        }
    }
    for (int ii = 0; ii < pointCountPerRow - 1; ii++) {
        for (int jj = 0; jj < pointCountPerRow - 1; jj++) {
            int v00 = ii * pointCountPerRow + jj;
            int v01 = ii * pointCountPerRow + (jj + 1);
            int v10 = (ii + 1) * pointCountPerRow + jj;
            int v11 = (ii + 1) * pointCountPerRow + (jj + 1);
            // first triangle
            meshData.indices.push_back(v00);
            meshData.indices.push_back(v01);
            meshData.indices.push_back(v10);
            // second triangle
            meshData.indices.push_back(v01);
            meshData.indices.push_back(v11);
            meshData.indices.push_back(v10);
        }
    }
    return meshData;
}

}  // namespace physika::common