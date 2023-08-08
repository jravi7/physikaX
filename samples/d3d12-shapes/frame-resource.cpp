//#include <memory>

#include "frame-resource.h"

#include "graphics/helpers.h"

namespace physika {

FrameResource::FrameResource(graphics::ID3D12DevicePtr pDevice, uint32_t const objectCount)
{
    //! Initialize Command Allocator
    fenceIndex = 0;
    graphics::ThrowIfFailed(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator)));
    perPassConstantBuffer = std::make_unique<graphics::UploadBuffer<PerPassCBData>>(pDevice, 1, true);

    perObjectCBData = std::make_unique<graphics::UploadBuffer<PerObjectCBData>>(pDevice, objectCount, true);
}

}  // namespace physika
