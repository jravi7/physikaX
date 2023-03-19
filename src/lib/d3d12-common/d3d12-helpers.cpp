#include "d3d12-common/d3d12-helpers.h"

#include <comdef.h>  // _com_error
#include <d3dx12.h>

#include "d3d12-common/d3d12-common.h"
#include "logger/logger.h"

namespace physika {
namespace d3d12_common {

char const* HRErrorDescription(HRESULT hr)
{
    _com_error err(hr);
    return err.ErrorMessage();
}
void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr)) {
        physika::logger::LOG_ERROR("%s", HRErrorDescription(hr));
        throw std::exception();
    }
}

ID3D12ResourcePtr CreateBuffer(ID3D12DevicePtr pDevice, ID3D12GraphicsCommandListPtr pCmdList,
                               D3D12_HEAP_PROPERTIES const& heapProperties, uint64_t const size)
{
    ID3D12ResourcePtr pBuffer = nullptr;

    if (!pDevice || !pCmdList || size == 0) {
        return pBuffer;
    }
    auto const& bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
    ThrowIfFailed(pDevice->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
                                                   &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                   nullptr, IID_PPV_ARGS(pBuffer.GetAddressOf())));
    return pBuffer;
}

DefaultGPUBuffer CreateDefaultBuffer(ID3D12DevicePtr pDevice, ID3D12GraphicsCommandListPtr pCmdList,
                                     void const* initialData, uint64_t const byteSize)
{
    if (!pDevice || !pCmdList || !initialData || byteSize == 0) {
        return { nullptr, nullptr };
    }
    auto const& defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto const& uploadHeapProperties  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    ID3D12ResourcePtr gpuBuffer = CreateBuffer(pDevice, pCmdList, defaultHeapProperties, byteSize);
    ID3D12ResourcePtr uploadBuffer =
        CreateBuffer(pDevice, pCmdList, uploadHeapProperties, byteSize);

    // Subresource Data desc
    D3D12_SUBRESOURCE_DATA subresourceData;
    subresourceData.pData      = initialData;
    subresourceData.RowPitch   = byteSize;
    subresourceData.SlicePitch = byteSize;

    auto const& barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        gpuBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
    pCmdList->ResourceBarrier(1, &barrier);

    // Issue copy
    UpdateSubresources<1>(pCmdList.Get(), gpuBuffer.Get(), uploadBuffer.Get(), 0, 0, 1,
                          &subresourceData);

    // Transition to generic read state
    auto const& barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
        gpuBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
    pCmdList->ResourceBarrier(1, &barrier2);

    return { gpuBuffer, uploadBuffer };
}

OutputBuffer CreateOutputBuffer(ID3D12DevicePtr pDevice, uint64_t const byteSize)
{
    if (!pDevice || byteSize == 0) {
        return { nullptr, nullptr };
    }

    ID3D12ResourcePtr gpuBuffer    = nullptr;
    ID3D12ResourcePtr uploadBuffer = nullptr;

    auto const& defaultHeapProperties  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto const& readbackHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
    auto const& bufferDesc             = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    ThrowIfFailed(pDevice->CreateCommittedResource(
        &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
        nullptr, IID_PPV_ARGS(gpuBuffer.GetAddressOf())));

    //! Resources in this heap must be created with D3D12_RESOURCE_STATE_COPY_DEST, and cannot be
    //! changed away from this.
    ThrowIfFailed(pDevice->CreateCommittedResource(
        &readbackHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

    return { gpuBuffer, uploadBuffer };
}

}  // namespace d3d12_common
}  // namespace physika