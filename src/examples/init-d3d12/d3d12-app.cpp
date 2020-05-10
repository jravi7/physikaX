

#include "d3d12-app.h"

#include <assert.h>
#include <comdef.h>
#include <d3dx12.h>

#include <iostream>

#include "logger/logger.h"

#define DEBUG_LAYER 1

using namespace Microsoft;
using namespace physika::logger;

namespace {
void ErrorDescription(HRESULT hr)
{
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hr))
        hr = HRESULT_CODE(hr);
    TCHAR* szErrMsg;

    if (FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
            hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&szErrMsg, 0,
            NULL) != 0) {
        _tprintf(TEXT("%s"), szErrMsg);
        LocalFree(szErrMsg);
    } else
        _tprintf(TEXT("[Could not find a description for error # %#x.]\n"), hr);
}
}  // namespace

namespace d3d12_sandbox {

D3D12App::D3D12App(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height),
      mSwapChainBufferCount{ 2 },
      mBackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM },
      mCurrentBackBuffer{ 0 }

{
}

bool D3D12App::Initialize()
{
    if (!physika::Application::Initialize()) {
        return false;
    }

    //! Enumerate graphics devices
    if (!EnumerateAdapters()) {
        return false;
    }

    //! Enabled D3D12 Debug Layer
#if defined(DEBUG_LAYER)
    if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&mDebugController)))) {
        LOG_WARN("Failed to load D3D12 Debug Layer.");
        mDebugController->EnableDebugLayer();
    }
#endif

    //! Create graphics device
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0,
                                 IID_PPV_ARGS(&mDevice)))) {
        LOG_FATAL("Failed to create D3D12 device.");
        return false;
    }

    //! Get max descriptor count
    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    LOG_INFO("Graphics Device Initialized.");
    LOG_INFO("Descriptors count.");
    LOG_INFO("RTV Descriptors. ");
    LOG_INFO("DSV Descriptors. ");
    LOG_INFO("CBV/SRV Descriptors. ");

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
    qualityLevels.Format      = mBackBufferFormat;
    qualityLevels.SampleCount = 8;
    qualityLevels.Flags       = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    if (!FAILED(mDevice->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels,
            sizeof(qualityLevels)))) {
        LOG_INFO("MSAA Quality level: %d", qualityLevels.NumQualityLevels);
    }

    //! Create CommandQueue and CommandAllocators
    if (!CreateCommandObjects()) {
        return false;
    }
    LOG_INFO("Command objects successfully created.");

    if (!CreateSwapChain()) {
        return false;
    }
    LOG_INFO("Swap chain successfully created.");

    if (!CreateDescriptorHeaps()) {
        return false;
    }
    LOG_INFO("Descriptor Heap created successfully");

    return true;
}
bool D3D12App::Shutdown()
{
    if (!physika::Application::Shutdown()) {
        return false;
    }
    return true;
}

bool D3D12App::EnumerateAdapters()
{
    IDXGIAdapter* pAdapter = nullptr;
    if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&mFactory)))) {
        assert(0 && "Failed to create a DXGI Factory instance");
        LOG_FATAL("Failed to create a DXGI Factory instance");
        return false;
    }
    for (uint32_t i = 0;
         mFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC desc{};
        pAdapter->GetDesc(&desc);

        std::wcout << desc.Description << std::endl;
    }
    return true;
}

bool D3D12App::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;

    if (FAILED(mDevice->CreateCommandQueue(&queueDesc,
                                           IID_PPV_ARGS(&mCommandQueue)))) {
        return false;
    }
    if (FAILED(mDevice->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(mCommandAllocator.GetAddressOf())))) {
        return false;
    }
    if (FAILED(mDevice->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr,
            IID_PPV_ARGS(mGraphicsCommandList.GetAddressOf())))) {
        return false;
    }

    mGraphicsCommandList->Close();
    return true;
}

bool D3D12App::CreateSwapChain()
{
    mSwapChain.Reset();
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    // Back buffer description
    swapChainDesc.BufferDesc.Width                   = mWidth;
    swapChainDesc.BufferDesc.Height                  = mHeight;
    swapChainDesc.BufferDesc.Format                  = mBackBufferFormat;
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering =
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    //! Sample Desc
    swapChainDesc.SampleDesc.Count   = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    //! Other properties
    swapChainDesc.Windowed     = true;
    swapChainDesc.BufferCount  = mSwapChainBufferCount;
    swapChainDesc.OutputWindow = mHwnd;
    swapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr = mFactory->CreateSwapChain(mCommandQueue.Get(), &swapChainDesc,
                                           &mSwapChain);
    if (FAILED(hr)) {
        ErrorDescription(hr);
        LOG_FATAL("Failed to create a DXGI SwapChain");

        return false;
    } else {
        LOG_INFO("SwapChain initialized");
    }

    return true;
}

bool D3D12App::CreateDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = mSwapChainBufferCount;
    rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask       = 0;

    HRESULT hr =
        mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
    if (FAILED(hr)) {
        LOG_FATAL("Failed to create Render Target Descriptor Heap");
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    ;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask       = 0;

    hr = mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
    if (FAILED(hr)) {
        LOG_FATAL("Failed to create Depth Stencil Descriptor Heap");
        return false;
    }

    return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12App::CurrentBackBufferView() const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrentBackBuffer,
        mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3D12App::DepthStencilView() const
{
    return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
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