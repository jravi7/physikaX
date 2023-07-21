#pragma once

#include <d3d12.h>
#include <dxgi1_3.h>
#include <stdint.h>  // uint32_t

#include <memory>  // unique_ptr
#include <vector>

#include "app-framework/application.h"
#include "app-framework/input.h"
#include "d3d12-common/d3d12-common.h"
#include "d3d12-common/d3d12-helpers.h"
#include "frame-resource.h"
#include "timer/timer.h"
#include "utility/camera.h"

namespace sample {

using physika::Keycode;
using physika::MouseButton;
namespace d3d12 = physika::d3d12_common;

struct InputStates
{
    bool  isMouseDown                      = {};
    bool  keyState[physika::kKeyCodeCount] = {};
    float mouseX                           = {};
    float mouseY                           = {};
};

class D3D12Shapes : public physika::Application
{
public:
    D3D12Shapes(TCHAR const* const title, int width, int height);
    ~D3D12Shapes();

    bool Initialize();
    bool Shutdown();

    void OnUpdate() override;
    void OnResize(int width, int height) override;
    void OnKeyUp(Keycode key) override;
    void OnKeyDown(Keycode key) override;
    void OnMouseUp(MouseButton button, int x, int y) override;
    void OnMouseDown(MouseButton button, int x, int y) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseWheel(int delta) override;

private:
    void InitializeDeviceObjects();
    void InitializeCommandObjects();

    // RTs
    void InitializeSwapChain();
    void CreateDepthStencilBuffer();

    // Resources
    void InitializeSceneCamera();
    void InitializeSceneGeometry();
    void InitializeFrameResources();
    void InitializeRenderItems();
    void CreateDescriptorHeaps();
    void CreateDescriptorViews();
    void CreateConstantBufferViews();
    void CreateRenderTargetViews();
    void ResizeViewportAndScissorRect();
    void CreateRootSignatures();
    void InitializePSOs();

    void FlushCommandQueue();
    void Update();
    void Draw();

    void ProcessKeyStates();

    uint32_t    mSwapChainBufferCount;
    uint32_t    mCurrentBackBuffer;
    DXGI_FORMAT mBackBufferFormat;

    // Device Objects
    d3d12::ID3D12DevicePtr              mD3D12Device;
    d3d12::IDXGIFactory1Ptr             mDXGIFactory;
    d3d12::IDXGIAdapter1Ptr             mDXGIAdapter;
    d3d12::ID3D12CommandQueuePtr        mCommandQueue;
    d3d12::ID3D12GraphicsCommandListPtr mGraphicsCommandList;
    d3d12::ID3D12CommandAllocatorPtr    mCommandAllocator;
    d3d12::ID3D12Debug1Ptr              mD3D12DebugController;
    d3d12::ID3D12DescriptorHeapPtr      mRtvHeap;
    d3d12::ID3D12DescriptorHeapPtr      mDsvHeap;
    d3d12::ID3D12DescriptorHeapPtr      mCBVHeap;

    //! Window surface objects
    d3d12::IDXGISwapChainPtr              mSwapChain;
    std::vector<d3d12::ID3D12ResourcePtr> mSwapChainBackBuffers;
    d3d12::ID3D12ResourcePtr              mDepthStencilBuffer;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    //! Sync Variables
    physika::Timer        mTimer;
    uint64_t              mCurrentFrameIndex;
    uint64_t              mFenceValue;
    d3d12::ID3D12FencePtr mFence;

    //! Resources
    d3d12::ID3D12PipelineStatePtr                        mPipelineState;
    d3d12::ID3D12RootSignaturePtr                        mRootSignature;
    std::vector<std::shared_ptr<physika::FrameResource>> mFrameResources;
    std::shared_ptr<physika::FrameResource>              mCurrentFrameResource;

    //! Scene resources
    physika::PerPassCBData                                        mPerPassCBData;
    std::unordered_map<std::string, std::shared_ptr<d3d12::Mesh>> mMeshBuffers;
    std::vector<std::shared_ptr<physika::RenderItem>>             mSceneObjects;
    physika::utility::Camera                                      mCamera;

    //! Input
    InputStates mInputStates;
};

}  // namespace sample
