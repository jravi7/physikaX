#pragma once
#include <d3d12.h>
#include <inttypes.h>    // uint64_t
#include <wrl/client.h>  // ComPtr

#include <tuple>
#include <unordered_map>

#include "d3d12-common/d3d12-common.h"

namespace physika {
namespace d3d12_common {

//! @brief Alias representing a tuple of gpubuffer and its upload heap buffer
using DefaultGPUBuffer = std::tuple<ID3D12ResourcePtr, ID3D12ResourcePtr>;

//! @brief Alias representing a tuple of gpubuffer and its read back heap buffer
using OutputBuffer = std::tuple<ID3D12ResourcePtr, ID3D12ResourcePtr>;

//! @brief Returns a string representation of a HRESULT param.
char const* HRErrorDescription(HRESULT hr);

//! @brief throw an exception on HRESULT failure
void ThrowIfFailed(HRESULT hr);

//! @brief Create a committed buffer
//! @out
ID3D12ResourcePtr CreateBuffer(ID3D12DevicePtr pDevice, D3D12_HEAP_PROPERTIES const& heapProperties,
                               uint64_t const size);

//! @brief Creates a default GPU Buffer and Upload Heap buffer with initial data.
DefaultGPUBuffer CreateDefaultBuffer(ID3D12DevicePtr pDevice, ID3D12GraphicsCommandListPtr pCmdlist,
                                     void const* initialData, uint64_t const byteSize);

//! @brief create a default GPU buffer and a readback buffer
OutputBuffer CreateOutputBuffer(ID3D12DevicePtr device, uint64_t const byteSize);

//! @brief Calculate size in bytes with given alignment
uint64_t GetSizeWithAlignment(uint64_t const byteSize, uint64_t const alignment);

struct Submesh
{
    uint32_t indexCount          = 0;
    uint32_t vertexStartLocation = 0;
    uint32_t indexStartLocation  = 0;
};

struct Mesh
{
    std::string                              name;
    std::unordered_map<std::string, Submesh> submeshes;

    ID3DBlobPtr vertexBufferCPU = nullptr;
    ID3DBlobPtr indexBufferCPU  = nullptr;

    ID3D12ResourcePtr vertexBufferGPU = nullptr;
    ID3D12ResourcePtr indexBufferGPU  = nullptr;

    ID3D12ResourcePtr vertexBufferUploadHeap = nullptr;
    ID3D12ResourcePtr indexBufferUploadHeap  = nullptr;

    DXGI_FORMAT indexFormat          = DXGI_FORMAT_UNKNOWN;
    UINT        indexBufferByteSize  = 0;
    uint32_t    vertexByteStride     = 0;
    uint32_t    vertexBufferByteSize = 0;

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
    void DisposeUploaders()
    {
        vertexBufferUploadHeap = nullptr;
        indexBufferUploadHeap  = nullptr;
    }
};

}  // namespace d3d12_common
}  // namespace physika