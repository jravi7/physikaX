#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "graphics/helpers.h"
#include "graphics/types.h"
#include "graphics/upload-buffer.h"
#include "renderer/types.h"

namespace physika {

template <typename T>
using UploadBufferPtr = std::unique_ptr<physika::graphics::UploadBuffer<T>>;

/*
 * Constant Buffers are aligned at a 16 byte boundary.
 * https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules#more-aggressive-packing
 */
struct PerPassCBData
{
    DirectX::SimpleMath::Matrix view;              // 64
    DirectX::SimpleMath::Matrix projection;        // 64
    DirectX::SimpleMath::Matrix viewProjection;    // 64
    float                       totalTime = 0.0;   // 4
    float                       deltaTime = 0.0f;  // 4
    float                       pad;               // 4
    DirectX::XMFLOAT4           pad2;              // 16
    DirectX::XMFLOAT3X3         pad3;              // 36
};

struct PerObjectCBData
{
    DirectX::SimpleMath::Matrix worldMatrix;  // 64
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
    DirectX::SimpleMath::Matrix worldMatrix;
};

struct FrameResource
{
public:
    FrameResource(graphics::ID3D12DevicePtr pDevice, uint32_t const objectCount);
    FrameResource()                           = delete;
    FrameResource(FrameResource const& other) = delete;
    FrameResource& operator=(FrameResource const& other) = delete;

    uint64_t                            fenceIndex            = 0;
    graphics::ID3D12CommandAllocatorPtr pCommandAllocator     = nullptr;
    UploadBufferPtr<PerPassCBData>      perPassConstantBuffer = nullptr;
    UploadBufferPtr<PerObjectCBData>    perObjectCBData       = nullptr;
};
}  // namespace physika