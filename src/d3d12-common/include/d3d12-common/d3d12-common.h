#include <d3d12.h>
#include <dxgi1_3.h>
#include <comdef.h>

namespace dx12{
#define SMART_COMPTR_DECL(interface) _COM_SMARTPTR_TYPEDEF(interface,__uuidof(interface))

SMART_COMPTR_DECL(ID3D12Device);
SMART_COMPTR_DECL(IDXGIFactory1);
SMART_COMPTR_DECL(IDXGIAdapter1);
SMART_COMPTR_DECL(ID3D12CommandQueue);
SMART_COMPTR_DECL(ID3D12GraphicsCommandList);
SMART_COMPTR_DECL(ID3D12CommandAllocator);
SMART_COMPTR_DECL(ID3D12Debug1);
SMART_COMPTR_DECL(ID3D12DescriptorHeap);
SMART_COMPTR_DECL(ID3D12Resource);

}