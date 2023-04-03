#pragma once
#include <DirectXMath.h>

#include "d3d12-common/d3d12-upload-buffer.h"

namespace physika {
struct PerObjectCBData
{
    DirectX::XMFLOAT4X4 mvp;
};

struct FrameResource
{
public:
    FrameResource(d3d12_common::ID3D12DevicePtr pDevice, uint32_t const objectCount);
    FrameResource()                           = delete;
    ~FrameResource()                          = default;
    FrameResource(FrameResource const& other) = delete;
    FrameResource& operator=(FrameResource const& other) = delete;

    uint64_t                                fenceIndex                                = 0;
    d3d12_common::ID3D12CommandAllocatorPtr pCommandAllocator                         = nullptr;
    std::unique_ptr<physika::d3d12_common::UploadBuffer<PerObjectCBData>> perObjectCB = nullptr;
};
}  // namespace physika