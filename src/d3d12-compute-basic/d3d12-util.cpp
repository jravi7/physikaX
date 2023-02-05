#include "d3d12-util.h"

#include <d3dx12.h>

namespace physika {
namespace d3d12_util {

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

DefaultGPUBuffer CreateDefaultBuffer(wrl::ComPtr<ID3D12Device>              pDevice,
                                     wrl::ComPtr<ID3D12GraphicsCommandList> pCmdList,
                                     void const* initialData, uint64_t const byteSize)
{
    if (!pDevice || !pCmdList || !initialData || byteSize == 0) {
        return { nullptr, nullptr };
    }

    D3D12ResourcePtr gpuBuffer    = nullptr;
    D3D12ResourcePtr uploadBuffer = nullptr;

    auto const& defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto const& uploadHeapProperties  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto const& bufferDesc            = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    ThrowIfFailed(pDevice->CreateCommittedResource(
        &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
        nullptr, IID_PPV_ARGS(gpuBuffer.GetAddressOf())));
    //! Resources created in a D3D12_HEAP_TYPE_UPLOAD heap must be created with
    //! D3D12_RESOURCE_STATE_GENERIC_READ and cannot be changed away from this.
    ThrowIfFailed(pDevice->CreateCommittedResource(
        &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

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

OutputBuffer CreateOutputBuffer(wrl::ComPtr<ID3D12Device> pDevice, uint64_t const byteSize)
{
    if (!pDevice || byteSize == 0) {
        return { nullptr, nullptr };
    }

    D3D12ResourcePtr gpuBuffer    = nullptr;
    D3D12ResourcePtr uploadBuffer = nullptr;

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
}  // namespace d3d12_util
}  // namespace physika