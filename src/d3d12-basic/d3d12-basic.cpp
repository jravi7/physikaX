#include "d3d12-basic.h"

#include <stdio.h>

#include <string>

#include "helpers.h"
#include "logger/logger.h"

namespace {

void PrintHeader(char const* message)
{
    size_t const headerSize = strnlen(message, 1024);
    std::string  decoration(headerSize, '-');
    printf("%s\n", decoration.c_str());
    printf("%s\n", message);
    printf("%s\n", decoration.c_str());
}

}  // namespace

namespace sample {

using namespace physika;

D3D12Basic::D3D12Basic(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height),
      mBackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM },
      mSwapChainBufferCount{ 2 }
{
}

D3D12Basic::~D3D12Basic()
{
}

bool D3D12Basic::Initialize()
{
    if (!physika::Application::Initialize()) {
        return false;
    }

    logger::SetApplicationName("D3D12 Basic");
    PrintHeader("Initializing...");
    if (!InitializeDeviceFactory()) {
        logger::LOG_FATAL("Failed to create a DXGI Factory object");
        return false;
    }

    if (!InitializeGraphicsDevice()) {
        logger::LOG_FATAL("Failed to create a D3D12 Device");
        return false;
    }

    return true;
}

bool D3D12Basic::InitializeDeviceFactory()
{
    UINT dxgiCreateFlags = 0;
#ifdef _DEBUG
    dxgiCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
    WRL::ComPtr<ID3D12Debug> dc0;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dc0)))) {
        if (SUCCEEDED(dc0->QueryInterface(IID_PPV_ARGS(&mD3D12DebugController)))) {
            logger::LOG_INFO("Enabling Debug Layer and GPU Based Validation.");
            mD3D12DebugController->EnableDebugLayer();
            mD3D12DebugController->SetEnableGPUBasedValidation(true);
        }
    }
#endif
    if (FAILED(CreateDXGIFactory2(dxgiCreateFlags, IID_PPV_ARGS(&mDXGIFactory)))) {
        logger::LOG_ERROR("Failed to a create DXGI Factory object.");
        return false;
    }
    return true;
}

bool D3D12Basic::InitializeGraphicsDevice()
{
    logger::LOG_INFO("Enumerating Adapters and Initializing Graphics Device");
    bool foundDevice = false;
    for (UINT ii = 0; mDXGIFactory->EnumAdapters1(ii, &mDXGIAdapter) != DXGI_ERROR_NOT_FOUND;
         ++ii) {
        DXGI_ADAPTER_DESC1 adapterDesc;
        mDXGIAdapter->GetDesc1(&adapterDesc);
        if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            auto refCount = mDXGIAdapter.Reset();
            if (refCount > 0) {
                logger::LOG_WARN("Dangling DXGI Adapter at index %d with refcount %llu", ii,
                                 refCount);
            }
            continue;
        }

        auto result = D3D12CreateDevice(mDXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0,
                                        IID_PPV_ARGS(&mD3D12Device));
        if (SUCCEEDED(result)) {
            foundDevice = true;
            // D3D12 capable device available.
            break;
        }
        // mDXGI
        mDXGIAdapter.Reset();
    }
    if (foundDevice) {
        logger::LOG_INFO("D3D12 Device Initialized");
    }
    return foundDevice;
}

bool D3D12Basic::Shutdown()
{
    if (!physika::Application::Shutdown()) {
        return false;
    }
    return true;
}

void D3D12Basic::OnUpdate()
{
}

void D3D12Basic::OnResize(int /*width*/, int /*height*/)
{
}

void D3D12Basic::OnKeyUp(Keycode /*key*/)
{
}

void D3D12Basic::OnKeyDown(Keycode /*key*/)
{
}

void D3D12Basic::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12Basic::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12Basic::OnMouseMove(int /*x*/, int /*y*/)
{
}

void D3D12Basic::OnMouseWheel(int /*delta*/)
{
}

}  // namespace sample