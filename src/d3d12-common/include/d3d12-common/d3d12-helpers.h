#include <d3d12.h>
#include <inttypes.h>    // uint64_t
#include <wrl/client.h>  // ComPtr

#include <tuple>

#include "d3d12-common/d3d12-common.h"

namespace physika {
namespace d3d12_common {

//! @brief Alias representing a tuple of gpubuffer and its upload heap buffer
using DefaultGPUBuffer = std::tuple<ID3D12ResourcePtr, ID3D12ResourcePtr>;

//! @brief Returns a string representation of a HRESULT param.
char const* HRErrorDescription(HRESULT hr);

//! @brief throw an exception on HRESULT failure
void ThrowIfFailed(HRESULT hr);

//! @brief Creates a default GPU Buffer and Upload Heap buffer with initial data.
//! @out
ID3D12ResourcePtr CreateBuffer(ID3D12DevicePtr pDevice, ID3D12GraphicsCommandListPtr pCmdList,
                               D3D12_HEAP_PROPERTIES const& heapProperties, uint64_t const size);

//! @brief Creates a default GPU Buffer and Upload Heap buffer with initial data.
//! @out
DefaultGPUBuffer CreateDefaultBuffer(ID3D12DevicePtr pDevice, ID3D12GraphicsCommandListPtr pCmdlist,
                                     void const* initialData, uint64_t const byteSize);

struct Mesh
{
    ID3D12ResourcePtr vertexBufferGPU;
    ID3D12ResourcePtr vertexBufferUploadHeap;
    ID3D12ResourcePtr indexBufferGPU;
    ID3D12ResourcePtr indexBufferUploadHeap;

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

}  // namespace d3d12_common
}  // namespace physika