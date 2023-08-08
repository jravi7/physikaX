#pragma once
#include <comdef.h>
#include <d3d12.h>
#include <dxgi1_3.h>
#include <wrl/client.h>  // ComPtr

namespace physika::graphics {
#define DEFINE_SMART_COMPTR(interface) using interface##Ptr = Microsoft::WRL::ComPtr<interface>;

DEFINE_SMART_COMPTR(ID3D12Device)
DEFINE_SMART_COMPTR(IDXGIFactory1)
DEFINE_SMART_COMPTR(IDXGIAdapter1)
DEFINE_SMART_COMPTR(ID3D12CommandQueue)
DEFINE_SMART_COMPTR(ID3D12GraphicsCommandList)
DEFINE_SMART_COMPTR(ID3D12CommandAllocator)
DEFINE_SMART_COMPTR(ID3D12Debug1)
DEFINE_SMART_COMPTR(ID3D12DescriptorHeap)
DEFINE_SMART_COMPTR(ID3D12Resource)
DEFINE_SMART_COMPTR(IDXGISwapChain)
DEFINE_SMART_COMPTR(ID3D12PipelineState)
DEFINE_SMART_COMPTR(ID3D12RootSignature)
DEFINE_SMART_COMPTR(ID3D12Fence)
DEFINE_SMART_COMPTR(ID3D12Debug)
DEFINE_SMART_COMPTR(ID3DBlob)

}  // namespace physika::graphics