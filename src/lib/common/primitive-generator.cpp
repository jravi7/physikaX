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
    meshData.vertices.push_back(
        { XMFLOAT3(side * 0.5f, -side * 0.5f, 0.0f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back({ XMFLOAT3(0, side * 0.5f, 0.0f), XMFLOAT4(Colors::Green) });
    meshData.vertices.push_back(
        { XMFLOAT3(-side * 0.5f, -side * 0.5f, 0.0f), XMFLOAT4(Colors::Magenta) });

    meshData.indices.push_back(0);
    meshData.indices.push_back(1);
    meshData.indices.push_back(2);

    return meshData;
}
MeshData CreateCube(float const side)
{
    MeshData meshData;
    if (std::abs(side - 0.0f) < std::numeric_limits<float>::epsilon()) {
        return meshData;
    }

    meshData.vertices.push_back(
        { XMFLOAT3(side * 0.5f, side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back(
        { XMFLOAT3(-side * 0.5f, side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back(
        { XMFLOAT3(-side * 0.5f, -side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back(
        { XMFLOAT3(side * 0.5f, -side * 0.5f, side * 0.5f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back(
        { XMFLOAT3(side * 0.5f, side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back(
        { XMFLOAT3(-side * 0.5f, side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back(
        { XMFLOAT3(-side * 0.5f, -side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) });
    meshData.vertices.push_back(
        { XMFLOAT3(side * 0.5f, -side * 0.5f, -side * 0.5f), XMFLOAT4(Colors::Blue) });

    // add face
    meshData.indices.push_back(0);
    meshData.indices.push_back(1);
    meshData.indices.push_back(2);
    meshData.indices.push_back(0);
    meshData.indices.push_back(2);
    meshData.indices.push_back(3);

    meshData.indices.push_back(4);
    meshData.indices.push_back(5);
    meshData.indices.push_back(1);
    meshData.indices.push_back(4);
    meshData.indices.push_back(1);
    meshData.indices.push_back(0);

    meshData.indices.push_back(4);
    meshData.indices.push_back(0);
    meshData.indices.push_back(3);
    meshData.indices.push_back(4);
    meshData.indices.push_back(3);
    meshData.indices.push_back(7);

    meshData.indices.push_back(5);
    meshData.indices.push_back(4);
    meshData.indices.push_back(7);
    meshData.indices.push_back(5);
    meshData.indices.push_back(7);
    meshData.indices.push_back(6);

    meshData.indices.push_back(1);
    meshData.indices.push_back(5);
    meshData.indices.push_back(6);
    meshData.indices.push_back(1);
    meshData.indices.push_back(6);
    meshData.indices.push_back(2);

    meshData.indices.push_back(3);
    meshData.indices.push_back(2);
    meshData.indices.push_back(6);
    meshData.indices.push_back(3);
    meshData.indices.push_back(6);
    meshData.indices.push_back(7);

    return meshData;
}

}  // namespace physika::common