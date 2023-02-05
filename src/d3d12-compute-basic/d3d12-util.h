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

//! @brief Type alias for a comptr of a D3D12Resource
using D3D12ResourcePtr = wrl::ComPtr<ID3D12Resource>;

//! @brief Alias representing a tuple of gpubuffer and its upload heap buffer
using DefaultGPUBuffer = std::tuple<D3D12ResourcePtr, D3D12ResourcePtr>;

//! @brief Alias representing a tuple of gpubuffer and its upload heap buffer
using OutputBuffer = std::tuple<D3D12ResourcePtr, D3D12ResourcePtr>;

//! @brief Returns a string representation of a HRESULT param.
char const* HRErrorDescription(HRESULT hr);

//! @brief throw an exception on HRESULT failure
void ThrowIfFailed(HRESULT hr);

//! @brief Creates a default GPU Buffer and Upload Heap buffer with initial data.
//! @out
DefaultGPUBuffer CreateDefaultBuffer(wrl::ComPtr<ID3D12Device>              device,
                                     wrl::ComPtr<ID3D12GraphicsCommandList> cmdlist,
                                     void const* initialData, uint64_t const byteSize);

OutputBuffer CreateOutputBuffer(wrl::ComPtr<ID3D12Device> device, uint64_t const byteSize);

}  // namespace d3d12_util
}  // namespace physika