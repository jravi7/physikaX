#pragma once

#include "common/primitive-generator.h"

#include <DirectXColors.h>
#include <SimpleMath.h>

#include <cmath>   //fabs
#include <limits>  //numeric_limits

namespace physika::common {

using namespace DirectX;

/*
        All Primitives are centered at the origin and use LHS

         y+
         ^
         |   z+
         |  /
         | /
         |/
         +---------> x+

         Vertex Data
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 normal;
            DirectX::XMFLOAT3 tangent;
            DirectX::XMFLOAT2 texcoord;
            DirectX::XMFLOAT4 color;
*/

MeshData CreateEquilateralTriangle(float const side)
{
    MeshData meshData;
    float    halfSide = side * 0.5f;

    meshData.vertices = {
        { XMFLOAT3(halfSide, -halfSide, 0.0f), XMFLOAT3(0, 0, -1.0), XMFLOAT3(0), XMFLOAT2(0.5, -0.5), XMFLOAT4(Colors::Blue) },
        { XMFLOAT3(0, halfSide, 0.0f), XMFLOAT3(0, 0, -1.0), XMFLOAT3(0), XMFLOAT2(0, 0.5), XMFLOAT4(Colors::Green) },
        { XMFLOAT3(-halfSide, -halfSide, 0.0f), XMFLOAT3(0, 0, -1.0), XMFLOAT3(0), XMFLOAT2(-0.5, -0.5), XMFLOAT4(Colors::Red) }
    };

    meshData.indices = { 2, 1, 0 };
    return meshData;
}

MeshData CreateCube(float const side)
{
    MeshData meshData;
    float    half = side * 0.5f;

    meshData.vertices.resize(24);
    // front
    meshData.vertices[0] = VertexData(XMFLOAT3(-half, -half, -half), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(0.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[1] = VertexData(XMFLOAT3(-half, +half, -half), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[2] = VertexData(XMFLOAT3(+half, +half, -half), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(1.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[3] = VertexData(XMFLOAT3(+half, -half, -half), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(1.0f, 1.0f), XMFLOAT4(Colors::Blue));

    // back
    meshData.vertices[4] = VertexData(XMFLOAT3(-half, -half, +half), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(1.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[5] = VertexData(XMFLOAT3(+half, -half, +half), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(0.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[6] = VertexData(XMFLOAT3(+half, +half, +half), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[7] = VertexData(XMFLOAT3(-half, +half, +half), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                      XMFLOAT2(1.0f, 0.0f), XMFLOAT4(Colors::Blue));

    // top
    meshData.vertices[8]  = VertexData(XMFLOAT3(-half, +half, -half), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(0.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[9]  = VertexData(XMFLOAT3(-half, +half, +half), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[10] = VertexData(XMFLOAT3(+half, +half, +half), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(1.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[11] = VertexData(XMFLOAT3(+half, +half, -half), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(1.0f, 1.0f), XMFLOAT4(Colors::Blue));

    // bottom
    meshData.vertices[12] = VertexData(XMFLOAT3(-half, -half, -half), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(1.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[13] = VertexData(XMFLOAT3(+half, -half, -half), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(0.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[14] = VertexData(XMFLOAT3(+half, -half, +half), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[15] = VertexData(XMFLOAT3(-half, -half, +half), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
                                       XMFLOAT2(1.0f, 0.0f), XMFLOAT4(Colors::Blue));

    // left
    meshData.vertices[16] = VertexData(XMFLOAT3(-half, -half, +half), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
                                       XMFLOAT2(0.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[17] = VertexData(XMFLOAT3(-half, +half, +half), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
                                       XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[18] = VertexData(XMFLOAT3(-half, +half, -half), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
                                       XMFLOAT2(1.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[19] = VertexData(XMFLOAT3(-half, -half, -half), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
                                       XMFLOAT2(1.0f, 1.0f), XMFLOAT4(Colors::Blue));

    // right
    meshData.vertices[20] = VertexData(XMFLOAT3(+half, -half, -half), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
                                       XMFLOAT2(0.0f, 1.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[21] = VertexData(XMFLOAT3(+half, +half, -half), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
                                       XMFLOAT2(0.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[22] = VertexData(XMFLOAT3(+half, +half, +half), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
                                       XMFLOAT2(1.0f, 0.0f), XMFLOAT4(Colors::Blue));
    meshData.vertices[23] = VertexData(XMFLOAT3(+half, -half, +half), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
                                       XMFLOAT2(1.0f, 1.0f), XMFLOAT4(Colors::Blue));

    meshData.indices.resize(36);

    // clang-format off
    // front
    meshData.indices[0] = 0; meshData.indices[1] = 1; meshData.indices[2] = 2;
    meshData.indices[3] = 0; meshData.indices[4] = 2; meshData.indices[5] = 3;

    // back
    meshData.indices[6] = 4; meshData.indices[7]  = 5; meshData.indices[8]  = 6;
    meshData.indices[9] = 4; meshData.indices[10] = 6; meshData.indices[11] = 7;

    // top
    meshData.indices[12] = 8; meshData.indices[13] =  9; meshData.indices[14] = 10;
    meshData.indices[15] = 8; meshData.indices[16] = 10; meshData.indices[17] = 11;

    // bottom
    meshData.indices[18] = 12; meshData.indices[19] = 13; meshData.indices[20] = 14;
    meshData.indices[21] = 12; meshData.indices[22] = 14; meshData.indices[23] = 15;

    // left
    meshData.indices[24] = 16; meshData.indices[25] = 17; meshData.indices[26] = 18;
    meshData.indices[27] = 16; meshData.indices[28] = 18; meshData.indices[29] = 19;

    // right
    meshData.indices[30] = 20; meshData.indices[31] = 21; meshData.indices[32] = 22;
    meshData.indices[33] = 20; meshData.indices[34] = 22; meshData.indices[35] = 23;
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
    int      halfSide         = side / 2;
    int      cellCountPerRow  = side / cellSize;
    int      pointCountPerRow = cellCountPerRow + 1;
    for (int z = halfSide; z >= -halfSide; z -= cellSize) {
        for (int x = -halfSide; x <= halfSide; x += cellSize) {
            VertexData vertex = { XMFLOAT3(float(x), 0, float(z)), XMFLOAT3(), XMFLOAT3(),
                                  XMFLOAT2(float(x) / float(side), float(z) / float(side)), XMFLOAT4(Colors::White) };
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

    // Calculate normals
    for (int ii = 0; ii < meshData.indices.size(); ii += 3) {
        unsigned int idx1 = meshData.indices[ii];
        unsigned int idx2 = meshData.indices[ii + 1];
        unsigned int idx3 = meshData.indices[ii + 2];

        SimpleMath::Vector3 const& A = meshData.vertices[idx1].position;
        SimpleMath::Vector3 const& B = meshData.vertices[idx2].position;
        SimpleMath::Vector3 const& C = meshData.vertices[idx3].position;

        SimpleMath::Vector3 const AB    = B - A;
        SimpleMath::Vector3 const AC    = C - A;
        SimpleMath::Vector3 const cross = AB.Cross(AC);

        SimpleMath::Vector3 n1 = meshData.vertices[idx1].normal;
        SimpleMath::Vector3 n2 = meshData.vertices[idx2].normal;
        SimpleMath::Vector3 n3 = meshData.vertices[idx3].normal;

        n1 += cross;
        n2 += cross;
        n3 += cross;

        meshData.vertices[idx1].normal = n1;
        meshData.vertices[idx2].normal = n2;
        meshData.vertices[idx3].normal = n3;
    }

    // normalize normals
    for (int ii = 0; ii < meshData.vertices.size(); ++ii) {
        SimpleMath::Vector3 n(meshData.vertices[ii].normal);
        n.Normalize();
        meshData.vertices[ii].normal = n;
    }

    return meshData;
}

}  // namespace physika::common