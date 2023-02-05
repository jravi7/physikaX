#include "d3d12-compute-basic.h"

#include <DirectXColors.h>
#include <d3dx12.h>
#include <stdio.h>

#include <filesystem>
#include <string>

#include "d3d12-util.h"
#include "d3dcompiler.h"
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

std::filesystem::path GetCurrentExeFullPath()
{
    int const kMaxPath         = 1024;
    char      buffer[kMaxPath] = { '\0' };
    GetModuleFileNameA(nullptr, buffer, kMaxPath - 1);
    return std::filesystem::path(buffer).parent_path();
}

}  // namespace

namespace sample {

using namespace physika;

D3D12ComputeBasic::D3D12ComputeBasic(TCHAR const* const title, int width, int height)
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

D3D12ComputeBasic::~D3D12ComputeBasic()
{
}

bool D3D12ComputeBasic::Initialize()
{
    if (!physika::Application::Initialize()) {
        return false;
    }

    logger::SetApplicationName("D3D12 Basic");
    logger::SetLoggingLevel(logger::LogLevel::kInfo);
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

    //! Prepare commandlist for resource loading.

    InitializePSOs();
    mCommandAllocator->Reset();
    mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr);
    InitializeResources();

    mGraphicsCommandList->Close();
    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
    FlushCommandQueue();

    ResizeViewportAndScissorRect();
    // std::get<1>(mVectorOfNumbers1) = nullptr;
    // std::get<1>(mVectorOfNumbers2) = nullptr;
    ComputeVectorAdd();

    return true;
}

bool D3D12ComputeBasic::InitializeDeviceObjects()
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

bool D3D12ComputeBasic::InitializeCommandObjects()
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

bool D3D12ComputeBasic::InitializeSwapChain()
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

bool D3D12ComputeBasic::CreateDescriptorHeaps()
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

bool D3D12ComputeBasic::CreateRenderTargetView()
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

bool D3D12ComputeBasic::CreateDepthStencilBufferAndView()
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

void D3D12ComputeBasic::ResizeViewportAndScissorRect()
{
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mViewport.Width    = static_cast<float>(mWindowWidth);
    mViewport.Height   = static_cast<float>(mWindowHeight);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

    mScissorRect = { 0, 0, mWindowWidth, mWindowHeight };
}

bool D3D12ComputeBasic::InitializeSyncObjects()
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

void D3D12ComputeBasic::InitializeResources()
{
    //! Initialize large arrays for data
    std::vector<float> numbersToAdd1 = {};
    std::vector<float> numbersToAdd2 = {};

    for (int ii = 0; ii < 32; ++ii) {
        numbersToAdd1.push_back(float(ii));
        numbersToAdd2.push_back(float(ii));
    }

    uint64_t byteSize = numbersToAdd1.size() * sizeof(float);

    mVectorOfNumbers1 = d3d12_util::CreateDefaultBuffer(mD3D12Device, mGraphicsCommandList,
                                                        numbersToAdd1.data(), byteSize);

    mVectorOfNumbers2 = d3d12_util::CreateDefaultBuffer(mD3D12Device, mGraphicsCommandList,
                                                        numbersToAdd2.data(), byteSize);

    mVectorAddResult = d3d12_util::CreateOutputBuffer(mD3D12Device, byteSize);
}

void D3D12ComputeBasic::InitializePSOs()
{
    CD3DX12_ROOT_PARAMETER slotRootParameter[3];
    slotRootParameter[0].InitAsShaderResourceView(0);
    slotRootParameter[1].InitAsShaderResourceView(1);
    slotRootParameter[2].InitAsUnorderedAccessView(0);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(3, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_NONE);

    WRL::ComPtr<ID3DBlob> signature;
    WRL::ComPtr<ID3DBlob> error;
    d3d12_util::ThrowIfFailed(D3D12SerializeRootSignature(
        &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    d3d12_util::ThrowIfFailed(mD3D12Device->CreateRootSignature(0, signature->GetBufferPointer(),
                                                                signature->GetBufferSize(),
                                                                IID_PPV_ARGS(&mRootSignature)));

    WRL::ComPtr<ID3DBlob> csByteCode = nullptr;

    WRL::ComPtr<ID3DBlob> errors;
    auto const            shaderPath = GetCurrentExeFullPath() / "compute-shader.hlsl";

    HRESULT hr = D3DCompileFromFile(shaderPath.wstring().c_str(), nullptr, nullptr, "CSMain",
                                    "cs_5_0", 0, 0, &csByteCode, &errors);

    if (errors != nullptr)
        OutputDebugStringA((char*)errors->GetBufferPointer());
    d3d12_util::ThrowIfFailed(hr);

    // Describe and create the compute pipeline object (PSO).
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature                    = mRootSignature.Get();
    psoDesc.CS                                = CD3DX12_SHADER_BYTECODE(csByteCode.Get());
    psoDesc.Flags                             = D3D12_PIPELINE_STATE_FLAG_NONE;
    d3d12_util::ThrowIfFailed(
        mD3D12Device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));
}

bool D3D12ComputeBasic::Shutdown()
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

void D3D12ComputeBasic::OnUpdate()
{
    Draw();
}

void D3D12ComputeBasic::Update()
{
}

void D3D12ComputeBasic::Draw()
{
    d3d12_util::ThrowIfFailed(mCommandAllocator->Reset());

    d3d12_util::ThrowIfFailed(
        mGraphicsCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));

    auto currentBackBuffer = mSwapChainBackBuffers[mCurrentBackBuffer].Get();

    CD3DX12_RESOURCE_BARRIER transitionToRenderTargetState = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mGraphicsCommandList->ResourceBarrier(1, &transitionToRenderTargetState);
    auto rtvDescriptorSize =
        mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    auto currentRTV = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                                    mCurrentBackBuffer, rtvDescriptorSize);
    auto dsv        = mDsvHeap->GetCPUDescriptorHandleForHeapStart();
    mGraphicsCommandList->OMSetRenderTargets(1, &currentRTV, true, &dsv);
    //! Set Viewport and ScissorRect
    mGraphicsCommandList->RSSetViewports(1, &mViewport);
    mGraphicsCommandList->RSSetScissorRects(1, &mScissorRect);

    //! Clear back buffer and depth/stencil buffer
    auto const& clearColor = DirectX::XMVECTORF32({ 0.2f, 0.2f, 0.2f, 1.0f });
    mGraphicsCommandList->ClearRenderTargetView(currentRTV, clearColor, 0, nullptr);
    mGraphicsCommandList->ClearDepthStencilView(
        dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    CD3DX12_RESOURCE_BARRIER transitionToPresentState = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mGraphicsCommandList->ResourceBarrier(1, &transitionToPresentState);

    d3d12_util::ThrowIfFailed(mGraphicsCommandList->Close());

    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    d3d12_util::ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrentBackBuffer = (mCurrentBackBuffer + 1) % mSwapChainBufferCount;

    FlushCommandQueue();
}

void D3D12ComputeBasic::FlushCommandQueue()
{
    mFenceValue++;
    logger::LOG_DEBUG("Flushing command queue: %d", mFenceValue);
    d3d12_util::ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mFenceValue));

    if (mFence->GetCompletedValue() < mFenceValue) {
        logger::LOG_DEBUG("Fence completed value: %d", mFence->GetCompletedValue());

        d3d12_util::ThrowIfFailed(mFence->SetEventOnCompletion(mFenceValue, mFenceEventHandle));
        WaitForSingleObject(mFenceEventHandle, INFINITE);
    }
}

void D3D12ComputeBasic::OnResize(int width, int height)
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
    d3d12_util::ThrowIfFailed(mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr));

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
        d3d12_util::ThrowIfFailed(
            mSwapChain->GetBuffer(ii, IID_PPV_ARGS(&mSwapChainBackBuffers[ii])));
        mD3D12Device->CreateRenderTargetView(mSwapChainBackBuffers[ii].Get(), nullptr,
                                             rtvHeapHandle);
        rtvHeapHandle.Offset(ii, rtvDescriptorSize);
    }

    //! Recreate depth stencil buffer and view
    if (!CreateDepthStencilBufferAndView()) {
        logger::LOG_ERROR("Failed to recreate depth/stencil buffer view during resize");
    }

    d3d12_util::ThrowIfFailed(mGraphicsCommandList->Close());
    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    FlushCommandQueue();

    ResizeViewportAndScissorRect();
}

void D3D12ComputeBasic::ComputeVectorAdd()
{
    d3d12_util::ThrowIfFailed(mCommandAllocator->Reset());
    d3d12_util::ThrowIfFailed(
        mGraphicsCommandList->Reset(mCommandAllocator.Get(), mPipelineState.Get()));

    auto input1                 = std::get<0>(mVectorOfNumbers1).Get()->GetGPUVirtualAddress();
    auto input2                 = std::get<0>(mVectorOfNumbers2).Get()->GetGPUVirtualAddress();
    auto outputResource         = std::get<0>(mVectorAddResult);
    auto outputResourceReadback = std::get<1>(mVectorAddResult);
    auto output                 = outputResource.Get()->GetGPUVirtualAddress();

    mGraphicsCommandList->SetComputeRootSignature(mRootSignature.Get());
    mGraphicsCommandList->SetComputeRootShaderResourceView(0, input1);
    mGraphicsCommandList->SetComputeRootShaderResourceView(1, input2);
    mGraphicsCommandList->SetComputeRootUnorderedAccessView(2, output);
    mGraphicsCommandList->Dispatch(1, 1, 1);

    // Transition OutputBuffer to CopySource
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        outputResource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE);
    mGraphicsCommandList->ResourceBarrier(1, &barrier);

    // Copy to readback buffer
    mGraphicsCommandList->CopyResource(outputResourceReadback.Get(), outputResource.Get());

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        outputResource.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON);
    mGraphicsCommandList->ResourceBarrier(1, &barrier);

    d3d12_util::ThrowIfFailed(mGraphicsCommandList->Close());
    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    FlushCommandQueue();

    std::vector<float> result;
    result.resize(32);
    float* resultPtr = nullptr;
    outputResourceReadback->Map(0, nullptr, (void**)&resultPtr);
    std::memcpy(result.data(), resultPtr, sizeof(float) * 32);
    outputResourceReadback->Unmap(0, nullptr);
    return;
}

void D3D12ComputeBasic::OnKeyUp(Keycode /*key*/)
{
}

void D3D12ComputeBasic::OnKeyDown(Keycode /*key*/)
{
}

void D3D12ComputeBasic::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12ComputeBasic::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12ComputeBasic::OnMouseMove(int /*x*/, int /*y*/)
{
}

void D3D12ComputeBasic::OnMouseWheel(int /*delta*/)
{
}

}  // namespace sample