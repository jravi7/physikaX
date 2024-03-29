#include "d3d12-lights.h"

#include <DirectXColors.h>
#include <d3dx12.h>
#include <stdio.h>

#include <filesystem>
#include <string>

#include "core/logger.h"
#include "d3dcompiler.h"
#include "renderer/primitive-generator.h"
#include "renderer/types.h"

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

std::string const& shaderHlsl = "lighting.hlsl";

}  // namespace

namespace sample {

namespace dx = DirectX;
using namespace physika;
using namespace physika::core;

D3D12Lights::D3D12Lights(TCHAR const* const title, int width, int height) : Application(title, width, height)
{
    mSwapChainBufferCount = 2;
    mCurrentFrameIndex    = 0;
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
    mCurrentFrameResource = nullptr;
    mSwapChainBackBuffers.clear();
    mSwapChainBackBuffers.resize(mSwapChainBufferCount);
    mFrameResources.reserve(mSwapChainBufferCount);
}

D3D12Lights::~D3D12Lights()
{
}

bool D3D12Lights::Initialize()
{
    if (!Application::Initialize()) {
        return false;
    }

    logger::SetApplicationName("D3D12 Basic");
    logger::SetLoggingLevel(logger::LogLevel::kInfo);

    PrintHeader("Initializing...");
    InitializeDeviceObjects();
    InitializeCommandObjects();
    InitializeSwapChain();  // Swapchain depends on commandqueue.
    CreateDepthStencilBuffer();

    graphics::ThrowIfFailed(mD3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())));

    mCommandAllocator->Reset();
    mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr);

    InitializeSceneCamera();
    InitializeSceneGeometry();
    InitializeSceneMaterials();
    InitializeRenderItems();
    InitializeFrameResources();
    CreateDescriptorHeaps();
    CreateDescriptorViews();
    CreateRootSignatures();
    InitializePSOs();

    mGraphicsCommandList->Close();
    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
    FlushCommandQueue();

    ResizeViewportAndScissorRect();
    mTimer.Start();

    return true;
}

void D3D12Lights::InitializeDeviceObjects()
{
    UINT dxgiCreateFlags = 0;
#ifdef _DEBUG
    dxgiCreateFlags |= DXGI_CREATE_FACTORY_DEBUG;
    graphics::ID3D12DebugPtr dc0;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dc0)))) {
        if (SUCCEEDED(dc0->QueryInterface(IID_PPV_ARGS(&mD3D12DebugController)))) {
            logger::LOG_INFO("Enabling Debug Layer and GPU Based Validation.");
            mD3D12DebugController->EnableDebugLayer();
            mD3D12DebugController->SetEnableGPUBasedValidation(true);
        }
    }
#endif
    HRESULT hr = CreateDXGIFactory2(dxgiCreateFlags, IID_PPV_ARGS(&mDXGIFactory));
    graphics::ThrowIfFailed(hr);

    logger::LOG_INFO("Enumerating Adapters and Initializing Graphics Device");
    bool foundDevice = false;
    for (UINT ii = 0; mDXGIFactory->EnumAdapters1(ii, &mDXGIAdapter) != DXGI_ERROR_NOT_FOUND; ++ii) {
        DXGI_ADAPTER_DESC1 adapterDesc;
        mDXGIAdapter->GetDesc1(&adapterDesc);
        if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            auto refCount = mDXGIAdapter.Reset();
            if (refCount > 0) {
                logger::LOG_WARN("Dangling DXGI Adapter at index %d with refcount %llu", ii, refCount);
            }
            continue;
        }

        hr = D3D12CreateDevice(mDXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mD3D12Device));
        if (SUCCEEDED(hr)) {
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
}

void D3D12Lights::InitializeCommandObjects()
{
    //! Create CommandQueue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    HRESULT hr                         = mD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));
    graphics::ThrowIfFailed(hr);

    //! Create CommandAllocator
    hr = mD3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator));
    graphics::ThrowIfFailed(hr);

    //! Create CommandList
    hr = mD3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr,
                                         IID_PPV_ARGS(mGraphicsCommandList.GetAddressOf()));
    graphics::ThrowIfFailed(hr);
    mGraphicsCommandList->Close();

    logger::LOG_DEBUG("Command objects successfully created.");
}

void D3D12Lights::InitializeFrameResources()
{
    uint32_t objectCount   = static_cast<uint32_t>(mSceneObjects.size());
    uint32_t materialCount = static_cast<uint32_t>(mMaterials.size());
    for (uint32_t ii = 0; ii < mSwapChainBufferCount; ++ii) {
        mFrameResources.emplace_back(std::make_shared<FrameResource>(mD3D12Device, objectCount, materialCount));
        mFrameResources[ii]->pCommandAllocator->Reset();
    }
}

void D3D12Lights::InitializeSwapChain()
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

    HRESULT hr = mDXGIFactory->CreateSwapChain(mCommandQueue.Get(), &swapChainDesc,
                                               &mSwapChain);  // TODO: Consider CreateSwapChainForHwnd
    graphics::ThrowIfFailed(hr);

    logger::LOG_DEBUG("Swap chain successfully created.");
}
void D3D12Lights::CreateDepthStencilBuffer()
{
    PrintHeader("Setting up Depth/Stencil buffer and view");

    // Release any existing buffers
    if (mDepthStencilBuffer) {
        mDepthStencilBuffer.Reset();
    }

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
    HRESULT hr =
        mD3D12Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                              &optClearValue, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf()));
    graphics::ThrowIfFailed(hr);
}

void D3D12Lights::CreateDescriptorHeaps()
{
    PrintHeader("Setting up descriptor heaps");

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = mSwapChainBufferCount;
    rtvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask       = 0;

    HRESULT hr = mD3D12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap));
    graphics::ThrowIfFailed(hr);

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask       = 0;

    hr = mD3D12Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));
    graphics::ThrowIfFailed(hr);

    /*  <----------------Frame 0-------------------><----------------Frame 1------------------->
     * -----------------------------------------------------------------------------------------
     * | PerObjectCB | Material Data | perPassData | PerObjectCB | Material Data | perPassData |
     * -----------------------------------------------------------------------------------------
     */
    UINT numDescriptors = static_cast<UINT>(mSceneObjects.size() + mMaterials.size() + 1) *
                          mSwapChainBufferCount;  // Additional 1 descriptor is for per frame cb

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;

    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask       = 0;
    hr                         = mD3D12Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCBVHeap));
    graphics::ThrowIfFailed(hr);
}

void D3D12Lights::CreateDescriptorViews()
{
    CreateRenderTargetViews();
    CreateConstantBufferViews();
}

void D3D12Lights::CreateRenderTargetViews()
{
    PrintHeader("Setting up Render Target Views");

    for (uint32_t ii = 0; ii < mSwapChainBufferCount; ++ii) {
        HRESULT hr = mSwapChain->GetBuffer(ii, IID_PPV_ARGS(&mSwapChainBackBuffers[ii]));
        graphics::ThrowIfFailed(hr);
        auto rtvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto descriptorHandle =
            CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), ii, rtvDescriptorSize);
        mD3D12Device->CreateRenderTargetView(mSwapChainBackBuffers[ii].Get(), nullptr, descriptorHandle);
    }
    logger::LOG_DEBUG("Render Target Views successfully created");

    //! Create a view for the depth/stencil buffer
    mD3D12Device->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDsvHeap->GetCPUDescriptorHandleForHeapStart());

    logger::LOG_DEBUG("Successfully created depth/stencil buffer and view");
}

void D3D12Lights::CreateConstantBufferViews()
{
    // CBs need to be aligned to 256 byte boundary.
    size_t const perPassBufferSize     = graphics::GetSizeWithAlignment(sizeof(physika::PerPassCBData), 256);
    size_t const perObjectBufferSize   = graphics::GetSizeWithAlignment(sizeof(physika::PerObjectCBData), 256);
    size_t const perMaterialBufferSize = graphics::GetSizeWithAlignment(sizeof(renderer::MaterialCBData), 256);

    auto cbvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    mMaterialDescriptorOffset = (uint32_t)(mSceneObjects.size());
    mPerPassDescriptorOffset  = (uint32_t)(mSceneObjects.size() + mMaterials.size());
    mNumDescriptorsPerFrame   = (uint32_t)(mSceneObjects.size() + mMaterials.size() + 1);

    // descriptors for object transforms
    for (uint32_t frameIndex = 0; frameIndex < mSwapChainBufferCount; ++frameIndex) {
        for (uint32_t jj = 0; jj < (uint32_t)mSceneObjects.size(); ++jj) {
            uint32_t                  descriptorIndex = static_cast<uint32_t>(frameIndex * mNumDescriptorsPerFrame + jj);
            D3D12_GPU_VIRTUAL_ADDRESS cbGPUVA =
                mFrameResources[frameIndex]->perObjectCBData->Resource()->GetGPUVirtualAddress();
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
            desc.BufferLocation   = cbGPUVA + jj * perObjectBufferSize;
            desc.SizeInBytes      = static_cast<UINT>(perObjectBufferSize);
            auto descriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCBVHeap->GetCPUDescriptorHandleForHeapStart(),
                                                                  descriptorIndex, cbvDescriptorSize);
            mD3D12Device->CreateConstantBufferView(&desc, descriptorHandle);
        }
        // descriptors for material data
        for (auto const& material : mMaterials) {
            uint32_t descriptorIndex = static_cast<uint32_t>(frameIndex * mNumDescriptorsPerFrame + mMaterialDescriptorOffset +
                                                             material.second->cbHeapIndex);
            D3D12_GPU_VIRTUAL_ADDRESS cbGPUVA =
                mFrameResources[frameIndex]->perMaterialData->Resource()->GetGPUVirtualAddress();
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
            desc.BufferLocation   = cbGPUVA + material.second->cbHeapIndex * perMaterialBufferSize;
            desc.SizeInBytes      = static_cast<UINT>(perMaterialBufferSize);
            auto descriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCBVHeap->GetCPUDescriptorHandleForHeapStart(),
                                                                  descriptorIndex, cbvDescriptorSize);
            mD3D12Device->CreateConstantBufferView(&desc, descriptorHandle);
        }

        // descriptors for frame constant data
        int perPassIndex = frameIndex * mNumDescriptorsPerFrame + mPerPassDescriptorOffset;

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
        desc.BufferLocation = mFrameResources[frameIndex]->perPassConstantBuffer->Resource()->GetGPUVirtualAddress();
        desc.SizeInBytes    = static_cast<UINT>(perPassBufferSize);
        auto descriptorHandle =
            CD3DX12_CPU_DESCRIPTOR_HANDLE(mCBVHeap->GetCPUDescriptorHandleForHeapStart(), (INT)perPassIndex, cbvDescriptorSize);
        mD3D12Device->CreateConstantBufferView(&desc, descriptorHandle);
    }
}

void D3D12Lights::ResizeViewportAndScissorRect()
{
    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mViewport.Width    = static_cast<float>(mWindowWidth);
    mViewport.Height   = static_cast<float>(mWindowHeight);
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;

    mScissorRect = { 0, 0, mWindowWidth, mWindowHeight };
}

void D3D12Lights::InitializeSceneCamera()
{
    float        fov         = 60.0f;
    float        n           = 1.0f;     // near; the two names are already taken in a windows header
    float        f           = 1000.0f;  // far
    float        aspectRatio = static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight);
    dx::XMFLOAT3 position{ 0.0f, 20.0f, -20.0f };
    dx::XMFLOAT3 target{ 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };

    mCamera.SetCameraProperties(n, f, fov, aspectRatio);
    mCamera.SetLookAt(target, up, position);
}

void D3D12Lights::InitializeSceneGeometry()
{
    renderer::MeshData cubeMeshData = renderer::CreateCube(10);
    renderer::MeshData gridMeshData = renderer::CreateUniformGrid(128, 2);

    UINT cubeVertexOffset = 0;
    UINT cubeIndexOffset  = 0;

    UINT gridVertexOffset = static_cast<UINT>(cubeMeshData.vertices.size());
    UINT gridIndexOffset  = static_cast<UINT>(cubeMeshData.indices.size());

    auto const vertexBufferSize = static_cast<uint32_t>(cubeMeshData.VertexBufferSize() + gridMeshData.VertexBufferSize());
    auto const indexBufferSize  = static_cast<uint32_t>(cubeMeshData.IndexBufferSize() + gridMeshData.IndexBufferSize());

    //! Initialize Mesh - Vertex and Index Buffers;
    auto shapesBuffer  = std::make_shared<renderer::Mesh>();
    shapesBuffer->name = "primitives";

    D3DCreateBlob(vertexBufferSize, &shapesBuffer->vertexBufferCPU);
    D3DCreateBlob(indexBufferSize, &shapesBuffer->indexBufferCPU);

    // Copy to system memory D3DBlobs
    uint8_t* dst = reinterpret_cast<uint8_t*>(shapesBuffer->vertexBufferCPU->GetBufferPointer());
    memcpy(dst, cubeMeshData.vertices.data(),
           cubeMeshData.VertexBufferSize());  // cube vertex data
    memcpy(dst + cubeMeshData.VertexBufferSize(), gridMeshData.vertices.data(),
           gridMeshData.VertexBufferSize());  // grid vertex data

    dst = reinterpret_cast<uint8_t*>(shapesBuffer->indexBufferCPU->GetBufferPointer());
    memcpy(dst, cubeMeshData.indices.data(),
           cubeMeshData.IndexBufferSize());  // cube vertex data
    memcpy(dst + cubeMeshData.IndexBufferSize(), gridMeshData.indices.data(),
           gridMeshData.IndexBufferSize());  // grid vertex data

    std::tie(shapesBuffer->vertexBufferGPU, shapesBuffer->vertexBufferUploadHeap) = graphics::CreateDefaultBuffer(
        mD3D12Device, mGraphicsCommandList, shapesBuffer->vertexBufferCPU->GetBufferPointer(), vertexBufferSize);

    std::tie(shapesBuffer->indexBufferGPU, shapesBuffer->indexBufferUploadHeap) = graphics::CreateDefaultBuffer(
        mD3D12Device, mGraphicsCommandList, shapesBuffer->indexBufferCPU->GetBufferPointer(), indexBufferSize);

    renderer::Submesh cubeSubmesh;
    cubeSubmesh.indexCount          = (uint32_t)cubeMeshData.indices.size();
    cubeSubmesh.vertexStartLocation = cubeVertexOffset;
    cubeSubmesh.indexStartLocation  = cubeIndexOffset;

    renderer::Submesh gridSubmesh;
    gridSubmesh.indexCount          = (uint32_t)gridMeshData.indices.size();
    gridSubmesh.vertexStartLocation = gridVertexOffset;
    gridSubmesh.indexStartLocation  = gridIndexOffset;

    shapesBuffer->submeshes["cube"] = cubeSubmesh;
    shapesBuffer->submeshes["grid"] = gridSubmesh;

    shapesBuffer->vertexBufferByteSize = vertexBufferSize;
    shapesBuffer->indexBufferByteSize  = indexBufferSize;
    shapesBuffer->vertexByteStride     = static_cast<uint32_t>(renderer::MeshData::PerVertexDataSize());
    shapesBuffer->indexFormat          = DXGI_FORMAT_R32_UINT;

    mMeshBuffers[shapesBuffer->name] = shapesBuffer;

    //! Scene Lights
    renderer::Light sunLight;
    sunLight.position        = DirectX::XMFLOAT3(10, 20, -10);
    sunLight.direction       = DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f);
    sunLight.ambientStrength = DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f);
    sunLight.strength        = DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f);
    mDirectionalLights.push_back(sunLight);

    renderer::Light pointLight;
    pointLight.position        = DirectX::XMFLOAT3(10, 20, -10);
    pointLight.ambientStrength = DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f);
    pointLight.strength        = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
    pointLight.fallOffStart    = 10.0f;
    pointLight.fallOffend      = 70.0f;
    mPointLights.push_back(pointLight);

    renderer::Light spotLight;
    spotLight.position         = DirectX::XMFLOAT3(0, 50, 0);
    spotLight.direction        = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
    spotLight.ambientStrength  = DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f);
    spotLight.strength         = DirectX::XMFLOAT3(0.6f, 0.4f, 0.2f);
    spotLight.fallOffStart     = 10.0f;
    spotLight.fallOffend       = 70.0f;
    spotLight.innerCutOffAngle = cos(DirectX::XMConvertToRadians(12.0f));
    spotLight.outerCutOffAngle = cos(DirectX::XMConvertToRadians(17.5f));
    // mSpotLights.push_back(spotLight);
}

void D3D12Lights::InitializeSceneMaterials()
{
    auto tileMaterial            = std::make_shared<renderer::Material>();
    tileMaterial->name           = "tile";
    tileMaterial->cbHeapIndex    = 0;
    tileMaterial->diffuseAlbedo  = DirectX::XMFLOAT4(DirectX::Colors::LightGray);
    tileMaterial->fresnel        = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
    tileMaterial->roughness      = 0.8f;
    tileMaterial->numFramesDirty = mSwapChainBufferCount;

    auto steelMaterial            = std::make_shared<renderer::Material>();
    steelMaterial->name           = "steel";
    steelMaterial->cbHeapIndex    = 1;
    steelMaterial->diffuseAlbedo  = DirectX::XMFLOAT4(DirectX::Colors::LightSteelBlue);
    steelMaterial->fresnel        = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
    steelMaterial->roughness      = 0.6f;
    steelMaterial->numFramesDirty = mSwapChainBufferCount;

    mMaterials["tile"]  = tileMaterial;
    mMaterials["steel"] = steelMaterial;
}

void D3D12Lights::InitializeRenderItems()
{
    // Iterate the mesh buffers and create render items
    std::shared_ptr<renderer::Mesh> geo            = mMeshBuffers["primitives"];
    auto                            cubeRenderItem = std::make_shared<RenderItem>();
    cubeRenderItem->geometryBuffer                 = geo.get();
    cubeRenderItem->indexBufferStartLocation       = geo->submeshes["cube"].indexStartLocation;
    cubeRenderItem->vertexBufferStartLocation      = geo->submeshes["cube"].vertexStartLocation;
    cubeRenderItem->indexCount                     = geo->submeshes["cube"].indexCount;
    cubeRenderItem->primitiveTopology              = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    cubeRenderItem->worldMatrix                    = DirectX::SimpleMath::Matrix::CreateTranslation({ 0.0f, 10.0f, 0.0f });
    cubeRenderItem->numFramesDirty                 = mSwapChainBufferCount;
    cubeRenderItem->material                       = mMaterials["steel"].get();

    auto gridRenderItem                       = std::make_shared<RenderItem>();
    gridRenderItem->geometryBuffer            = geo.get();
    gridRenderItem->indexBufferStartLocation  = geo->submeshes["grid"].indexStartLocation;
    gridRenderItem->vertexBufferStartLocation = geo->submeshes["grid"].vertexStartLocation;
    gridRenderItem->indexCount                = geo->submeshes["grid"].indexCount;
    gridRenderItem->primitiveTopology         = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    gridRenderItem->worldMatrix               = DirectX::SimpleMath::Matrix::CreateTranslation({ 0.0f, -10.0f, 0.0f });
    gridRenderItem->numFramesDirty            = mSwapChainBufferCount;
    gridRenderItem->material                  = mMaterials["tile"].get();

    cubeRenderItem->objectIndex = (int)mSceneObjects.size();
    mSceneObjects.push_back(cubeRenderItem);

    gridRenderItem->objectIndex = (int)mSceneObjects.size();
    mSceneObjects.push_back(gridRenderItem);
}

void D3D12Lights::CreateRootSignatures()
{
    CD3DX12_DESCRIPTOR_RANGE1 cbvPerObjectRange;
    cbvPerObjectRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1 /*num*/, 0 /*baseRegister*/, 0 /*space*/);

    CD3DX12_DESCRIPTOR_RANGE1 cbvPerFrameRange;
    cbvPerFrameRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1 /*num*/, 1 /*baseRegister*/, 0 /*space*/);

    CD3DX12_DESCRIPTOR_RANGE1 cbvPerMaterialRange;
    cbvPerMaterialRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1 /*num*/, 2 /*baseRegister*/, 0 /*space*/);

    CD3DX12_ROOT_PARAMETER1 rootParameters[3]{};
    rootParameters[0].InitAsDescriptorTable(1, &cbvPerObjectRange, D3D12_SHADER_VISIBILITY_ALL);
    rootParameters[1].InitAsDescriptorTable(1, &cbvPerFrameRange, D3D12_SHADER_VISIBILITY_ALL);
    rootParameters[2].InitAsDescriptorTable(1, &cbvPerMaterialRange, D3D12_SHADER_VISIBILITY_ALL);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
                               D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    graphics::ID3DBlobPtr signature;
    graphics::ID3DBlobPtr error;
    graphics::ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error));
    graphics::ThrowIfFailed(mD3D12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                                              IID_PPV_ARGS(&mRootSignature)));
}
void D3D12Lights::InitializePSOs()
{
    graphics::ID3DBlobPtr  vsByteCode = nullptr;
    graphics::ID3DBlobPtr  psByteCode = nullptr;
    graphics::ID3DBlobPtr  errors;
    auto const             shaderPath   = GetCurrentExeFullPath() / shaderHlsl;
    std::string            numLightsStr = std::to_string(mDirectionalLights.size() + mSpotLights.size() + mPointLights.size());
    std::string            numPointLights       = std::to_string(mPointLights.size());
    std::string            numDirectionalLights = std::to_string(mDirectionalLights.size());
    std::string            numSpotLights        = std::to_string(mSpotLights.size());
    const D3D_SHADER_MACRO macros[]             = {
        "NUM_LIGHTS",
        numLightsStr.c_str(),
        "NUM_DIRECTIONAL_LIGHTS",
        numDirectionalLights.c_str(),  // Start index for point lights
        "NUM_POINT_LIGHTS",
        numPointLights.c_str(),  // Start index for spot lights
        "NUM_SPOT_LIGHTS",
        numSpotLights.c_str(),
        nullptr,
        nullptr,
    };

    HRESULT hr =
        D3DCompileFromFile(shaderPath.wstring().c_str(), macros, nullptr, "VSMain", "vs_5_0", 0, 0, &vsByteCode, &errors);

    if (errors != nullptr)
        OutputDebugStringA((char*)errors->GetBufferPointer());
    graphics::ThrowIfFailed(hr);

    hr = D3DCompileFromFile(shaderPath.wstring().c_str(), macros, nullptr, "PSMain", "ps_5_0", 0, 0, &psByteCode, &errors);
    if (errors != nullptr)
        OutputDebugStringA((char*)errors->GetBufferPointer());
    graphics::ThrowIfFailed(hr);

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout                        = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature                     = mRootSignature.Get();
    psoDesc.VS                                 = CD3DX12_SHADER_BYTECODE(vsByteCode.Get());
    psoDesc.PS                                 = CD3DX12_SHADER_BYTECODE(psByteCode.Get());
    psoDesc.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.FillMode           = D3D12_FILL_MODE_SOLID;
    psoDesc.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask                         = UINT_MAX;
    psoDesc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets                   = mSwapChainBufferCount;
    psoDesc.RTVFormats[0]                      = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat                          = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.SampleDesc.Count                   = 1;
    graphics::ThrowIfFailed(mD3D12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPipelineState)));
}

bool D3D12Lights::Shutdown()
{
    mTimer.Stop();
    FlushCommandQueue();
    if (!Application::Shutdown()) {
        return false;
    }
    return true;
}

void D3D12Lights::OnUpdate()
{
    ProcessKeyStates();
    Update();
    Draw();
}

void D3D12Lights::Update()
{
    //! Wait for frame resources to be freed up
    auto resourceIndex    = mCurrentFrameIndex % mSwapChainBufferCount;
    mCurrentFrameResource = mFrameResources[resourceIndex];
    if (mCurrentFrameResource->fenceIndex != 0 && mFence->GetCompletedValue() < mCurrentFrameResource->fenceIndex) {
        HANDLE eventHandle = CreateEvent(nullptr, false, false, nullptr);
        assert(eventHandle && "Failed to create handle");
        graphics::ThrowIfFailed(mFence->SetEventOnCompletion(mFenceValue, eventHandle));
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }

    //! Update Camera matrix
    physika::PerPassCBData perPassCBData;
    perPassCBData.deltaTime      = mTimer.Delta();
    perPassCBData.totalTime      = mTimer.TotalRunningTime();
    perPassCBData.view           = mCamera.View().Transpose();
    perPassCBData.projection     = mCamera.Projection().Transpose();
    perPassCBData.viewProjection = mCamera.ViewProjection().Transpose();
    perPassCBData.eyePosition    = mCamera.Position();
    int lightIndex               = 0;
    for (int ii = 0; ii < mDirectionalLights.size(); ii++) {
        memcpy(&perPassCBData.lights[lightIndex], &mDirectionalLights[ii], sizeof(renderer::Light));
        lightIndex++;
    }
    for (int ii = 0; ii < mPointLights.size(); ii++) {
        memcpy(&perPassCBData.lights[lightIndex], &mPointLights[ii], sizeof(renderer::Light));
        lightIndex++;
    }
    for (int ii = 0; ii < mSpotLights.size(); ii++) {
        memcpy(&perPassCBData.lights[lightIndex], &mSpotLights[ii], sizeof(renderer::Light));
        lightIndex++;
    }
    mCurrentFrameResource->perPassConstantBuffer->CopyData(0, perPassCBData);

    //! Update Object Data
    for (int ii = 0; ii < (int)mSceneObjects.size(); ++ii) {
        if (mSceneObjects[ii]->numFramesDirty <= 0) {
            continue;
        }
        PerObjectCBData                    perObjectCBData = { mSceneObjects[ii]->worldMatrix.Transpose() };
        DirectX::SimpleMath::Matrix const& m               = mSceneObjects[ii]->worldMatrix;
        // normal matrix calculation TODO: calculate only once.
        DirectX::XMFLOAT3X3 model3x3     = DirectX::XMFLOAT3X3(m._11, m._12, m._13, m._21, m._22, m._23, m._31, m._32, m._33);
        DirectX::XMMATRIX   model3x3SIMD = DirectX::XMLoadFloat3x3(&model3x3);
        model3x3SIMD                     = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, model3x3SIMD));
        DirectX::XMStoreFloat3x3(&perObjectCBData.normalMatrix, model3x3SIMD);
        // copy to cb
        mCurrentFrameResource->perObjectCBData->CopyData(ii, perObjectCBData);
        mSceneObjects[ii]->numFramesDirty--;
    }

    //! Update Material Data
    for (auto& material : mMaterials) {
        if (material.second->numFramesDirty <= 0) {
            continue;
        }
        renderer::MaterialCBData perMaterialData = {};
        perMaterialData.diffuseAlbedo            = material.second->diffuseAlbedo;
        perMaterialData.fresnel                  = material.second->fresnel;
        perMaterialData.roughness                = material.second->roughness;
        mCurrentFrameResource->perMaterialData->CopyData(material.second->cbHeapIndex, perMaterialData);
        material.second->numFramesDirty--;
    }

    mTimer.Tick();
}

void D3D12Lights::Draw()
{
    auto& pCommandAllocator = mCurrentFrameResource->pCommandAllocator;
    graphics::ThrowIfFailed(pCommandAllocator->Reset());

    graphics::ThrowIfFailed(mGraphicsCommandList->Reset(pCommandAllocator.Get(), nullptr));

    auto currentBackBuffer = mSwapChainBackBuffers[mCurrentBackBuffer].Get();

    CD3DX12_RESOURCE_BARRIER transitionToRenderTargetState = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mGraphicsCommandList->ResourceBarrier(1, &transitionToRenderTargetState);
    auto rtvDescriptorSize = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    auto currentRTV =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrentBackBuffer, rtvDescriptorSize);
    auto dsv = mDsvHeap->GetCPUDescriptorHandleForHeapStart();
    mGraphicsCommandList->OMSetRenderTargets(1, &currentRTV, true, &dsv);
    //! Set root signature and pipeline state
    mGraphicsCommandList->SetGraphicsRootSignature(mRootSignature.Get());
    mGraphicsCommandList->SetPipelineState(mPipelineState.Get());

    //! Set Descriptor Heap
    ID3D12DescriptorHeap* descriptorHeaps[] = { mCBVHeap.Get() };
    mGraphicsCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    //! Set Root Descriptor
    int  frameResourceIndex = static_cast<int>(mCurrentFrameIndex % mSwapChainBufferCount);
    auto cbvDescriptorSize  = mD3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    //! Set per frame cb descriptor handle
    int  perPassIndex = frameResourceIndex * mNumDescriptorsPerFrame + mPerPassDescriptorOffset;
    auto gpuPerFrameCBDescriptorHandle =
        CD3DX12_GPU_DESCRIPTOR_HANDLE(mCBVHeap->GetGPUDescriptorHandleForHeapStart(), perPassIndex, cbvDescriptorSize);

    mGraphicsCommandList->SetGraphicsRootDescriptorTable(1, gpuPerFrameCBDescriptorHandle);

    //! Set Viewport and ScissorRect
    mGraphicsCommandList->RSSetViewports(1, &mViewport);
    mGraphicsCommandList->RSSetScissorRects(1, &mScissorRect);

    //! Clear back buffer and depth/stencil buffer
    auto const& clearColor = DirectX::XMVECTORF32({ 0.2f, 0.2f, 0.2f, 1.0f });
    mGraphicsCommandList->ClearRenderTargetView(currentRTV, clearColor, 0, nullptr);
    mGraphicsCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Setup mesh render
    for (int ii = 0; ii < mSceneObjects.size(); ++ii) {
        int objectIndex   = static_cast<int>(frameResourceIndex * mNumDescriptorsPerFrame + mSceneObjects[ii]->objectIndex);
        int materialIndex = static_cast<int>(frameResourceIndex * mNumDescriptorsPerFrame + mMaterialDescriptorOffset +
                                             mSceneObjects[ii]->material->cbHeapIndex);

        auto gpuPerObjectCBDescriptorHandle =
            CD3DX12_GPU_DESCRIPTOR_HANDLE(mCBVHeap->GetGPUDescriptorHandleForHeapStart(), objectIndex, cbvDescriptorSize);
        mGraphicsCommandList->SetGraphicsRootDescriptorTable(0, gpuPerObjectCBDescriptorHandle);

        auto materialCBDescriptorHandle =
            CD3DX12_GPU_DESCRIPTOR_HANDLE(mCBVHeap->GetGPUDescriptorHandleForHeapStart(), materialIndex, cbvDescriptorSize);
        mGraphicsCommandList->SetGraphicsRootDescriptorTable(2, materialCBDescriptorHandle);

        auto const& ibView                    = mSceneObjects[ii]->geometryBuffer->IndexBufferView();
        auto const& vbView                    = mSceneObjects[ii]->geometryBuffer->VertexBufferView();
        uint32_t    indexCount                = mSceneObjects[ii]->indexCount;
        uint32_t    vertexBufferStartLocation = mSceneObjects[ii]->vertexBufferStartLocation;
        uint32_t    indexBufferStartLocation  = mSceneObjects[ii]->indexBufferStartLocation;
        mGraphicsCommandList->IASetIndexBuffer(&ibView);
        mGraphicsCommandList->IASetVertexBuffers(0, 1, &vbView);
        mGraphicsCommandList->IASetPrimitiveTopology(mSceneObjects[ii]->primitiveTopology);
        mGraphicsCommandList->DrawIndexedInstanced(indexCount, 1, indexBufferStartLocation, vertexBufferStartLocation, 0);
    }

    CD3DX12_RESOURCE_BARRIER transitionToPresentState = CD3DX12_RESOURCE_BARRIER::Transition(
        currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mGraphicsCommandList->ResourceBarrier(1, &transitionToPresentState);

    graphics::ThrowIfFailed(mGraphicsCommandList->Close());

    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    graphics::ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrentBackBuffer = (mCurrentBackBuffer + 1) % mSwapChainBufferCount;

    mCurrentFrameIndex++;
    mFenceValue++;
    mCurrentFrameResource->fenceIndex = mFenceValue;
    graphics::ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mFenceValue));
}

void D3D12Lights::FlushCommandQueue()
{
    mFenceValue++;
    logger::LOG_DEBUG("Flushing command queue: %d", mFenceValue);
    graphics::ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mFenceValue));

    if (mFence->GetCompletedValue() < mFenceValue) {
        HANDLE fenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
        logger::LOG_DEBUG("Fence completed value: %d", mFence->GetCompletedValue());
        graphics::ThrowIfFailed(mFence->SetEventOnCompletion(mFenceValue, fenceEventHandle));
        WaitForSingleObject(fenceEventHandle, INFINITE);
    }
}

void D3D12Lights::OnResize(int width, int height)
{
    if (!mD3D12Device) {
        return;
    }
    auto  resourceIndex     = mCurrentFrameIndex % mSwapChainBufferCount;
    auto& pCommandAllocator = mFrameResources[resourceIndex]->pCommandAllocator;
    logger::LOG_DEBUG("%d x %d", width, height);
    //! Update client widt and height;
    mWindowWidth  = width;
    mWindowHeight = height;

    //! Flush the gpu before modifying any resource
    FlushCommandQueue();

    //! Reset the command list to prepare resources
    graphics::ThrowIfFailed(mGraphicsCommandList->Reset(pCommandAllocator.Get(), nullptr));

    //! Release previous swapchain resources
    for (auto& backBuffer : mSwapChainBackBuffers) {
        backBuffer.Reset();
    }
    mDepthStencilBuffer.Reset();

    //! Resize swapchain back buffer size and format
    mSwapChain->ResizeBuffers(mSwapChainBufferCount, mWindowWidth, mWindowHeight, mBackBufferFormat,
                              DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

    //! Reset current back buffer
    mCurrentBackBuffer = 0;

    //! Recreate Depth/Stencil buffer
    CreateDepthStencilBuffer();

    //! Recreate RenderTargetViews and depth stencil buffer and view
    CreateRenderTargetViews();

    graphics::ThrowIfFailed(mGraphicsCommandList->Close());
    ID3D12CommandList* commandLists[] = { mGraphicsCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    FlushCommandQueue();

    ResizeViewportAndScissorRect();
}

void D3D12Lights::OnKeyUp(Keycode key)
{
    mInputStates.keyState[key] = false;
}

void D3D12Lights::OnKeyDown(Keycode key)
{
    mInputStates.keyState[key] = true;
}

void D3D12Lights::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
    mInputStates.isMouseDown = false;
}

void D3D12Lights::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
    mInputStates.isMouseDown = true;
}

void D3D12Lights::OnMouseMove(int x, int y)
{
    if (mInputStates.isMouseDown) {
        float angularVelocity = 10.0f * mTimer.Delta();
        float xRotDeg         = DirectX::XMConvertToDegrees(mCamera.XRotation());
        float yRotDeg         = DirectX::XMConvertToDegrees(mCamera.YRotation());

        float dx = static_cast<float>(x) - mInputStates.mouseX;
        float dy = static_cast<float>(y) - mInputStates.mouseY;

        xRotDeg += dy * angularVelocity;
        yRotDeg += dx * angularVelocity;

        mCamera.SetXRotation(DirectX::XMConvertToRadians(xRotDeg));
        mCamera.SetYRotation(DirectX::XMConvertToRadians(yRotDeg));
    }
    mInputStates.mouseX = static_cast<float>(x);
    mInputStates.mouseY = static_cast<float>(y);
}

void D3D12Lights::OnMouseWheel(int /*delta*/)
{
}

void D3D12Lights::ProcessKeyStates()
{
    DirectX::SimpleMath::Vector3 offset   = mCamera.Position();
    float                        velocity = 10.0f * mTimer.Delta();

    if (mInputStates.keyState[kA]) {
        offset += -velocity * mCamera.Right();
    }

    if (mInputStates.keyState[kD]) {
        offset += velocity * mCamera.Right();
    }

    if (mInputStates.keyState[kS]) {
        offset += velocity * mCamera.Forward();
    }

    if (mInputStates.keyState[kW]) {
        offset += -velocity * mCamera.Forward();
    }
    mCamera.SetPosition(offset);
}

}  // namespace sample