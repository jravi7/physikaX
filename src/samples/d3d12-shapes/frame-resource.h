#pragma once
#include <DirectXMath.h>

#include "d3d12-common/d3d12-helpers.h"
#include "d3d12-common/d3d12-upload-buffer.h"

namespace physika {

template <typename T>
using UploadBufferPtr = std::unique_ptr<physika::d3d12_common::UploadBuffer<T>>;

struct PerPassCBData
{
    DirectX::XMFLOAT4X4 mvp;
};

struct RenderItem
{
    RenderItem()                                  = default;
    uint32_t            indexCount                = 0;
    uint32_t            vertexBufferStartLocation = 0;
    uint32_t            indexBufferStartLocation  = 0;
    d3d12_common::Mesh* parentMeshBuffer          = nullptr;
    DirectX::XMFLOAT4X4 worldMatrix;
};

struct FrameResource
{
public:
    FrameResource(d3d12_common::ID3D12DevicePtr pDevice, uint32_t const objectCount);
    FrameResource()                           = delete;
    FrameResource(FrameResource const& other) = delete;
    FrameResource& operator=(FrameResource const& other) = delete;

    uint64_t                                fenceIndex            = 0;
    d3d12_common::ID3D12CommandAllocatorPtr pCommandAllocator     = nullptr;
    UploadBufferPtr<PerPassCBData>          perPassConstantBuffer = nullptr;
};
}  // namespace physika