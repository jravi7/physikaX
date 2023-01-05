#include "d3d12-basic.h"

#include <DirectXColors.h>
#include <comdef.h>  // _com_error
#include <d3dx12.h>
#include <stdio.h>

#include <exception>
#include <string>

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

char const* HRErrorDescription(HRESULT hr)
{
    _com_error err(hr);
    return err.ErrorMessage();
}

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr)) {
        physika::logger::LOG_ERROR("%s", HRErrorDescription(hr));
        throw std::exception();
    }
}

}  // namespace

namespace sample {

using namespace physika;

D3D12Basic::D3D12Basic(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height)
{
    mSwapChainBufferCount = 2;
    mFenceValue           = 0;
    mCurrentBackBuffer    = 0;
    mBackBufferFormat     = DXGI_FORMAT_R8G8B8A8_UNORM;

    mViewport    = {};
    mScissorRect = {};

    mSwapChain            = nullptr;
    mD3D12Device          = nullptr;
    mDXGIFactory          = nullptr;
    mDXGIAdapter          = nullptr;
    mCommandQueue         = nullptr;
    mGraphicsCommandList  = nullptr;
    mCommandAllocator     = nullptr;
    mD3D12DebugController = nullptr;
    mRtvHeap              = nullptr;
    mDsvHeap              = nullptr;
    mDepthStencilBuffer   = nullptr;
    mFence                = nullptr;
    mSwapChainBackBuffers.clear();
    mSwapChainBackBuffers.resize(mSwapChainBufferCount);
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
    if (!InitializeDeviceObjects()) {
        logger::LOG_FATAL("Failed to create device objects");
        return false;
    }

    if (!InitializeCommandObjects()) {
        logger::LOG_FATAL("Failed to create a Command Objects");
        return false;
    }

    if (!InitializeSwapChain()) {
        logger::LOG_FATAL("Failed to create a SwapChain");
        return false;
    }

    if (!InitializeSyncObjects()) {
        logger::LOG_FATAL("Failed to create a Fence");
    }

    if (!CreateDescriptorHeaps()) {
        logger::LOG_FATAL("Failed to create Descriptor Heaps");
        return false;
    }

    if (!CreateRenderTargetView()) {
        logger::LOG_FATAL("Failed to create Render Target Views");
        return false;
    }

    if (!CreateDepthStencilBufferAndView()) {
        logger::LOG_FATAL("Failed to create a DepthStencil Buffer and View");
        return false;
    }

    return true;
}

bool D3D12Basic::InitializeDeviceObjects()
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

bool D3D12Basic::InitializeCommandObjects()
{
    //! Initialize Command Queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (FAILED(mD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)))) {
        logger::LOG_FATAL("Failed to create command queue.");
        return false;
    }

    //! Initialize Command Allocator
    if (FAILED(mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                    IID_PPV_ARGS(&mCommandAllocator)))) {
        logger::LOG_FATAL("Failed to create command queue.");
        return false;
    }

    if (FAILED(mD3D12Device->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr,
            IID_PPV_ARGS(mGraphicsCommandList.GetAddressOf())))) {
        return false;
    }

    mGraphicsCommandList->Close();

    logger::LOG_DEBUG("Command objects successfully created.");

    return true;
}

bool D3D12Basic::InitializeSwapChain()
{
    PrintHeader("Setting up SwapChain");

    mSwapChain.Reset();
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    // Back buffer description
    swapChainDesc.BufferDesc.Width                   = mWindowWidth;
    swapChainDesc.BufferDesc.Height                  = mWindowHeight;
    swapChainDesc.BufferDesc.Format                  = mBackBufferFormat;
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
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

    HRESULT hr = mDXGIFactory->CreateSwapChain(mCommandQueue.Get(), &swapChainDesc, &mSwapChain);
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create a DXGI SwapChain");
        return false;
    }

    logger::LOG_DEBUG("Swap chain successfully created.");

    return true;
}

bool D3D12Basic::CreateDescriptorHeaps()
{
    PrintHeader("Setting up descriptor heaps");

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = mSwapChainBufferCount;
    rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask       = 0;

    HRESULT hr = mD3D12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create Render Target Descriptor Heap");
        return false;
    }

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    ;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask       = 0;

    hr = mD3D12Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create Depth Stencil Descriptor Heap");
        return false;
    }

    logger::LOG_DEBUG("Descriptor Heap created successfully");

    return true;
}

bool D3D12Basic::CreateRenderTargetView()
{
    PrintHeader("Setting up Render Target Views");

    for (uint32_t ii = 0; ii < mSwapChainBufferCount; ++ii) {
        if (FAILED(mSwapChain->GetBuffer(ii, IID_PPV_ARGS(&mSwapChainBackBuffers[ii])))) {
            logger::LOG_FATAL("Failed to get swapchain back buffer.");
            return false;
        }
        auto rtvDescriptorSize =
            mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto descriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            mRtvHeap->GetCPUDescriptorHandleForHeapStart(), ii, rtvDescriptorSize);
        mD3D12Device->CreateRenderTargetView(mSwapChainBackBuffers[ii].Get(), nullptr,
                                             descriptorHandle);
    }
    logger::LOG_DEBUG("Render Target Views successfully created");
    return true;
}

bool D3D12Basic::CreateDepthStencilBufferAndView()
{
    PrintHeader("Setting up Depth/Stencil buffer and view");

    CD3DX12_HEAP_PROPERTIES heapProperties{ D3D12_HEAP_TYPE_DEFAULT };

    D3D12_RESOURCE_DESC desc;
    desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Alignment          = 0;
    desc.Width              = mWindowWidth;
    desc.Height             = mWindowHeight;
    desc.MipLevels          = 1;
    desc.DepthOrArraySize   = 1;
    desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClearValue;
    optClearValue.DepthStencil = { 1.0f, 0 };
    optClearValue.Format       = desc.Format;

    //! Create Depth Stencil buffer
    if (FAILED(mD3D12Device->CreateCommittedResource(
            &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &optClearValue, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())))) {
        logger::LOG_FATAL("Failed to create depth/stencil buffer");
        return false;
    }

    //! Create a view for the buffer
    mD3D12Device->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr,
                                         mDsvHeap->GetCPUDescriptorHandleForHeapStart());

    logger::LOG_DEBUG("Successfully created depth/stencil buffer and view");

    return true;
}

void D3D12Basic::ResizeViewportAndScissorRect()
{
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mViewport.Width    = static_cast<float>(mWindowWidth);
    mViewport.Height   = static_cast<float>(mWindowHeight);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

    mScissorRect = { 0, 0, mWindowWidth, mWindowHeight };
}

bool D3D12Basic::InitializeSyncObjects()
{
    if (FAILED(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)))) {
        return false;
    }
    mFenceEventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
    if (!mFenceEventHandle) {
        logger::LOG_FATAL("Failed to create a fence handle");
        return false;
    }
    return true;
}

bool D3D12Basic::Shutdown()
{
    FlushCommandQueue();
    if (CloseHandle(mFenceEventHandle)) {
        logger::LOG_ERROR("Failed to close event handle on shutdown");
        return false;
    }
    if (!physika::Application::Shutdown()) {
        return false;
    }
    return true;
}

void D3D12Basic::OnUpdate()
{
    Draw();
}

void D3D12Basic::Update()
{
}

void D3D12Basic::Draw()
{
    ThrowIfFailed(mCommandAllocator->Reset());

    ThrowIfFailed(mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr));

    auto currentBackBuffer = mSwapChainBackBuffers[mCurrentBackBuffer].Get();

    CD3DX12_RESOURCE_BARRIER transitionToRenderTargetState = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mGraphicsCommandList->ResourceBarrier(1, &transitionToRenderTargetState);

    //! Set Viewport and ScissorRect
    mGraphicsCommandList->RSSetViewports(1, &mViewport);
    mGraphicsCommandList->RSSetScissorRects(1, &mScissorRect);

    //! Clear back buffer and depth/stencil buffer
    auto rtvDescriptorSize =
        mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    auto currentRTV = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                                    mCurrentBackBuffer, rtvDescriptorSize);
    auto dsv        = mDsvHeap->GetCPUDescriptorHandleForHeapStart();
    mGraphicsCommandList->ClearRenderTargetView(currentRTV, DirectX::Colors::LightSteelBlue, 0,
                                                nullptr);
    mGraphicsCommandList->ClearDepthStencilView(
        dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    mGraphicsCommandList->OMSetRenderTargets(1, &currentRTV, true, &dsv);

    CD3DX12_RESOURCE_BARRIER transitionToPresentState = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mGraphicsCommandList->ResourceBarrier(1, &transitionToPresentState);

    ThrowIfFailed(mGraphicsCommandList->Close());

    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrentBackBuffer = (mCurrentBackBuffer + 1) % mSwapChainBufferCount;

    FlushCommandQueue();
}

void D3D12Basic::FlushCommandQueue()
{
    mFenceValue++;
    logger::LOG_DEBUG("Flushing command queue: %d", mFenceValue);
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mFenceValue));

    if (mFence->GetCompletedValue() < mFenceValue) {
        logger::LOG_DEBUG("Fence completed value: %d", mFence->GetCompletedValue());

        ThrowIfFailed(mFence->SetEventOnCompletion(mFenceValue, mFenceEventHandle));
        WaitForSingleObject(mFenceEventHandle, INFINITE);
    }
}

void D3D12Basic::OnResize(int width, int height)
{
    if (!mD3D12Device) {
        return;
    }
    logger::LOG_DEBUG("%d x %d", width, height);
    //! Update client widt and height;
    mWindowWidth  = width;
    mWindowHeight = height;

    //! Flush the gpu before modifying any resource
    FlushCommandQueue();

    //! Reset the command list to prepare resources
    ThrowIfFailed(mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr));

    //! Release previous swapchain resources
    for (auto& backBuffer : mSwapChainBackBuffers) {
        backBuffer.Reset();
    }
    mDepthStencilBuffer.Reset();

    //! Resize swapchain back buffer size and format
    mSwapChain->ResizeBuffers(mSwapChainBufferCount, mWindowWidth, mWindowHeight, mBackBufferFormat,
                              DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    mCurrentBackBuffer = 0;

    //! Recreate RenderTargetViews
    auto rtvDescriptorSize =
        mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (uint32_t ii = 0; ii < mSwapChainBufferCount; ++ii) {
        ThrowIfFailed(mSwapChain->GetBuffer(ii, IID_PPV_ARGS(&mSwapChainBackBuffers[ii])));
        mD3D12Device->CreateRenderTargetView(mSwapChainBackBuffers[ii].Get(), nullptr,
                                             rtvHeapHandle);
        rtvHeapHandle.Offset(ii, rtvDescriptorSize);
    }

    //! Recreate depth stencil buffer and view
    if (!CreateDepthStencilBufferAndView()) {
        logger::LOG_ERROR("Failed to recreate depth/stencil buffer view during resize");
    }

    ThrowIfFailed(mGraphicsCommandList->Close());
    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    FlushCommandQueue();

    ResizeViewportAndScissorRect();
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