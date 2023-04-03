#pragma once

#include <d3dx12.h>

#include "d3d12-common/d3d12-common.h"
#include "d3d12-common/d3d12-helpers.h"

namespace physika::d3d12_common {

template <typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12DevicePtr pDevice, uint32_t elementCount, bool isConstantBuffer = false)
    {
        mElementSize = sizeof(T);
        if (isConstantBuffer) {
            mElementSize = SizeWithAlignment(sizeof(T), 256);
        }
        auto const& uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        mBuffer = CreateBuffer(pDevice, uploadHeapProperties, mElementSize * elementCount);
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
        T* dst = mMappedBuffer + elementIndex;
        memcpy(pMappedBuffer, static_cast<void*>(&elementData), sizeof(T));
    }

private:
    ID3D12ResourcePtr mBuffer;
    uint64_t          mElementSize;
    T*                mMappedBuffer = nullptr;
};

}  // namespace physika::d3d12_common