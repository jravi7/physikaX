#pragma once

#include <d3dx12.h>

#include "graphics/helpers.h"
#include "graphics/types.h"

namespace physika::graphics {

template <typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12DevicePtr pDevice, uint32_t elementCount, bool isConstantBuffer = false)
    {
        mElementSize = sizeof(T);
        if (isConstantBuffer) {
            //! Constant Buffer minimum allocation size - 256 bytes.
            mElementSize = GetSizeWithAlignment(sizeof(T), 256);
        }
        auto const& uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        mBuffer                          = CreateBuffer(pDevice, uploadHeapProperties, mElementSize * elementCount);
        mBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedBuffer));
    }
    ~UploadBuffer()
    {
        mBuffer->Unmap(0, nullptr);
    }

    ID3D12Resource* Resource()
    {
        return mBuffer.Get();
    }

    void CopyData(int const elementIndex, T const& elementData)
    {
        uint8_t* dst = mMappedBuffer + elementIndex * mElementSize;
        memcpy(dst, &elementData, sizeof(T));
    }

private:
    ID3D12ResourcePtr mBuffer;
    uint64_t          mElementSize;
    uint8_t*          mMappedBuffer = nullptr;
};

}  // namespace physika::graphics