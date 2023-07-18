//#include <memory>

#include "frame-resource.h"

#include "d3d12-common/d3d12-helpers.h"

namespace physika {

FrameResource::FrameResource(d3d12_common::ID3D12DevicePtr pDevice, uint32_t const objectCount)
{
    //! Initialize Command Allocator
    fenceIndex = 0;
    d3d12_common::ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                                IID_PPV_ARGS(&pCommandAllocator)));
    perPassConstantBuffer =
        std::make_unique<d3d12_common::UploadBuffer<PerPassCBData>>(pDevice, objectCount, true);
}

}  // namespace physika
