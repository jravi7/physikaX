#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "graphics/helpers.h"
#include "graphics/upload-buffer.h"
#include "renderer/constant-data.h"
#include "renderer/types.h"

namespace physika {

constexpr int MAX_LIGHTS = 16;

template <typename T>
using UploadBufferPtr = std::unique_ptr<physika::graphics::UploadBuffer<T>>;

/*
 * Constant Buffers are aligned at a 16 byte boundary.
 * https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules#more-aggressive-packing
 */
struct PerPassCBData
{
    DirectX::XMFLOAT4X4 view;            // 64
    DirectX::XMFLOAT4X4 projection;      // 64
    DirectX::XMFLOAT4X4 viewProjection;  // 64
    DirectX::XMFLOAT3   eyePosition;
    float               totalTime = 0.0;     // 16
    float               deltaTime = 0.0f;    // 4
    DirectX::XMFLOAT3   pad;                 // 12
    renderer::Light     lights[MAX_LIGHTS];  // 64 x n
};

struct PerObjectCBData
{
    DirectX::XMFLOAT4X4 worldMatrix;   // 64
    DirectX::XMFLOAT3X3 normalMatrix;  // 64
};

struct RenderItem
{
    RenderItem()                                          = default;
    uint32_t                    numFramesDirty            = 0;
    int                         objectIndex               = -1;
    D3D_PRIMITIVE_TOPOLOGY      primitiveTopology         = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    uint32_t                    indexCount                = 0;
    uint32_t                    vertexBufferStartLocation = 0;
    uint32_t                    indexBufferStartLocation  = 0;
    renderer::Mesh*             geometryBuffer            = nullptr;
    renderer::Material*         material                  = nullptr;
    DirectX::SimpleMath::Matrix worldMatrix;
};

struct FrameResource
{
public:
    FrameResource(graphics::ID3D12DevicePtr pDevice, uint32_t const objectCount, uint32_t const materialCount);
    FrameResource()                           = delete;
    FrameResource(FrameResource const& other) = delete;
    FrameResource& operator=(FrameResource const& other) = delete;

    uint64_t                                  fenceIndex            = 0;
    graphics::ID3D12CommandAllocatorPtr       pCommandAllocator     = nullptr;
    UploadBufferPtr<PerPassCBData>            perPassConstantBuffer = nullptr;
    UploadBufferPtr<PerObjectCBData>          perObjectCBData       = nullptr;
    UploadBufferPtr<renderer::MaterialCBData> perMaterialData       = nullptr;
};
}  // namespace physika