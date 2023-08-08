//#include <memory>

#include "frame-resource.h"

#include "graphics/helpers.h"

namespace physika {

using namespace graphics;

FrameResource::FrameResource(ID3D12DevicePtr pDevice, uint32_t const objectCount, uint32_t const materialCount)
{
    //! Initialize Command Allocator
    fenceIndex = 0;
    ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator)));
    perPassConstantBuffer = std::make_unique<UploadBuffer<PerPassCBData>>(pDevice, 1, true);
    perObjectCBData       = std::make_unique<UploadBuffer<PerObjectCBData>>(pDevice, objectCount, true);
    perMaterialData       = std::make_unique<UploadBuffer<renderer::MaterialCBData>>(pDevice, materialCount, true);
}

}  // namespace physika
