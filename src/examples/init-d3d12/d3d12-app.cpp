#include <assert.h>
#include <dxgi.h>
#include <iostream>

#include "d3d12-app.h"

#define DEBUG_LAYER 0

using namespace Microsoft;

namespace {
void EnumerateAdapters()
{
    WRL::ComPtr<IDXGIFactory> pFactory = nullptr;
    IDXGIAdapter*             pAdapter = nullptr;
    if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&pFactory)))) {
        assert(0 && "Failed to create a DXGI Factory instance");
        std::cout << "Failed to create a DXGI Factory instance" << std::endl;
        return;
    }
    for (uint32_t i = 0;
         pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC desc{};
        pAdapter->GetDesc(&desc);

        std::wcout << desc.Description << std::endl;
    }
}
}  // namespace

namespace d3d12_sandbox {

D3D12App::D3D12App(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height)
{
}

bool D3D12App::Initialize()
{
    if (!physika::Application::Initialize()) {
        return false;
    }

    //! Enumerate graphics devices
    EnumerateAdapters();

    //! Enabled D3D12 Debug Layer
#if defined(DEBUG_LAYER)
    WRL::ComPtr<ID3D12Debug> debugController;
    if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
    }
#endif

    //! Create graphics device
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0,
                                 IID_PPV_ARGS(&mDevice)))) {
        return false;
    }

    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    std::cout << "Graphics Device Initialized" << std::endl;

    std::cout << "Descriptors count" << std::endl;
    std::cout << "RTV Descriptors " << mRtvDescriptorSize << std::endl;
    std::cout << "DSV Descriptors " << mDsvDescriptorSize << std::endl;
    std::cout << "CBV/SRV Descriptors " << mCbvSrvDescriptorSize << std::endl;

    return true;
}
bool D3D12App::Shutdown()
{
    if (!physika::Application::Shutdown()) {
        return false;
    }
    return true;
}

void D3D12App::OnUpdate()
{
}

void D3D12App::OnResize(int /*width*/, int /*height*/)
{
}

void D3D12App::OnKeyUp(Keycode /*key*/)
{
}

void D3D12App::OnKeyDown(Keycode /*key*/)
{
}

void D3D12App::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12App::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12App::OnMouseMove(int /*x*/, int /*y*/)
{
}

void D3D12App::OnMouseWheel(int /*delta*/)
{
}

}  // namespace d3d12_sandbox