#pragma once

#include <comdef.h>  // _com_error
#include <d3d12.h>
#include <inttypes.h>    // uint64_t
#include <wrl/client.h>  // ComPtr

#include <exception>
#include <tuple>

#include "logger/logger.h"

namespace physika {
namespace d3d12_util {

namespace wrl = Microsoft::WRL;

using D3D12ResourcePtr = wrl::ComPtr<ID3D12Resource>;

char const* HRErrorDescription(HRESULT hr);

//! @brief throw an exception on HRESULT failure
void ThrowIfFailed(HRESULT hr);

//! @brief Creates a default GPU Buffer and Upload Heap buffer with initial data.
//! @out
std::tuple<D3D12ResourcePtr, D3D12ResourcePtr> CreateDefaultBuffer(
    wrl::ComPtr<ID3D12Device> device, wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist,
    void const* initialData, uint64_t const byteSize);

struct Mesh
{
    D3D12ResourcePtr vertexBufferGPU;
    D3D12ResourcePtr vertexBufferUploadHeap;
    D3D12ResourcePtr indexBufferGPU;
    D3D12ResourcePtr indexBufferUploadHeap;

    DXGI_FORMAT indexFormat         = DXGI_FORMAT_R32_UINT;
    UINT        indexBufferByteSize = 0;
    uint32_t    vertexByteStride;
    uint32_t    vertexBufferByteSize;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes  = vertexByteStride;
        vbv.SizeInBytes    = vertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
        ibv.Format         = indexFormat;
        ibv.SizeInBytes    = indexBufferByteSize;

        return ibv;
    }
};

}  // namespace d3d12_util
}  // namespace physika