//#include <memory>

#include "frame-resource.h"

#include "d3d12-common/d3d12-helpers.h"

namespace physika {

using namespace d3d12_common;

FrameResource::FrameResource(ID3D12DevicePtr pDevice, uint32_t const objectCount, uint32_t const materialCount)
{
    //! Initialize Command Allocator
    fenceIndex = 0;
    ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator)));
    perPassConstantBuffer = std::make_unique<UploadBuffer<PerPassCBData>>(pDevice, 1, true);
    perObjectCBData       = std::make_unique<UploadBuffer<PerObjectCBData>>(pDevice, objectCount, true);
    perMaterialData       = std::make_unique<UploadBuffer<MaterialCBData>>(pDevice, materialCount, true);
}

}  // namespace physika
