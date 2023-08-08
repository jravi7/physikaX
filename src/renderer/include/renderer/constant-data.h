#pragma once

#include <DirectXMath.h>

#include "renderer/types.h"

namespace physika::renderer {

constexpr uint8_t kMaxLights = 16;

struct PerPassCBData
{
    DirectX::XMFLOAT4X4 view;            // 64
    DirectX::XMFLOAT4X4 projection;      // 64
    DirectX::XMFLOAT4X4 viewProjection;  // 64
    DirectX::XMFLOAT3   eyePosition;
    float               totalTime = 0.0;     // 16
    float               deltaTime = 0.0f;    // 4
    DirectX::XMFLOAT3   pad;                 // 12
    Light               lights[kMaxLights];  // 64 x n
};

struct ObjectCBData
{
    DirectX::XMFLOAT4X4 worldMatrix;   // 64
    DirectX::XMFLOAT4X4 normalMatrix;  // 64
};

struct MaterialCBData
{
    DirectX::XMFLOAT4 diffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 fresnel       = { 0.01f, 0.01f, 0.01f };
    float             roughness     = 0.25f;  // 0 being shiny and 1.0 for maximum roughness
};

}  // namespace physika::renderer