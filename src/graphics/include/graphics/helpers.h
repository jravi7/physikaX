#pragma once
#include <d3d12.h>
#include <inttypes.h>    // uint64_t
#include <wrl/client.h>  // ComPtr

#include <tuple>
#include <unordered_map>

#include "graphics/types.h"

namespace physika {
namespace graphics {

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
ID3D12ResourcePtr CreateBuffer(ID3D12DevicePtr pDevice, D3D12_HEAP_PROPERTIES const& heapProperties, uint64_t const size);

//! @brief Creates a default GPU Buffer and Upload Heap buffer with initial data.
DefaultGPUBuffer CreateDefaultBuffer(ID3D12DevicePtr pDevice, ID3D12GraphicsCommandListPtr pCmdlist, void const* initialData,
                                     uint64_t const byteSize);

//! @brief create a default GPU buffer and a readback buffer
OutputBuffer CreateOutputBuffer(ID3D12DevicePtr device, uint64_t const byteSize);

//! @brief Calculate size in bytes with given alignment
uint64_t GetSizeWithAlignment(uint64_t const byteSize, uint64_t const alignment);

}  // namespace graphics
}  // namespace physika