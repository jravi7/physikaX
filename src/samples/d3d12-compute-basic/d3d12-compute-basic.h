#pragma once

#include <d3d12.h>
#include <dxgi1_3.h>
#include <stdint.h>      // uint32_t
#include <wrl/client.h>  // ComPtr

#include <memory>  // unique_ptr
#include <vector>

#include "app-framework/application.h"
#include "app-framework/input.h"
#include "d3d12-common/d3d12-common.h"
#include "d3d12-common/d3d12-helpers.h"

namespace sample {

using physika::Keycode;
using physika::MouseButton;
namespace d3d12 = physika::d3d12_common;

class D3D12ComputeBasic : public physika::Application
{
public:
    D3D12ComputeBasic(TCHAR const* const title, int width, int height);
    ~D3D12ComputeBasic();

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
    bool InitializeDeviceObjects();
    bool InitializeCommandObjects();
    bool InitializeSwapChain();
    bool InitializeSyncObjects();

    bool CreateDescriptorHeaps();
    bool CreateRenderTargetView();
    bool CreateDepthStencilBufferAndView();
    void ResizeViewportAndScissorRect();

    // Resources
    void InitializeResources();
    void InitializePSOs();
    void ComputeVectorAdd();

    void FlushCommandQueue();

    void Update();
    void Draw();

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
    d3d12::ID3D12DescriptorHeapPtr      mSRVHeap;

    //! Window surface objects
    d3d12::IDXGISwapChainPtr              mSwapChain;
    std::vector<d3d12::ID3D12ResourcePtr> mSwapChainBackBuffers;
    d3d12::ID3D12ResourcePtr              mDepthStencilBuffer;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    //! Sync Variables
    uint64_t              mFenceValue;
    HANDLE                mFenceEventHandle;
    d3d12::ID3D12FencePtr mFence;

    //! Resources
    d3d12::ID3D12DescriptorHeapPtr mUAVHeap;
    d3d12::DefaultGPUBuffer        mVectorOfNumbers1;
    d3d12::DefaultGPUBuffer        mVectorOfNumbers2;
    d3d12::OutputBuffer            mVectorAddResult;
    d3d12::ID3D12ResourcePtr       mResultBuffer;
    d3d12::ID3D12PipelineStatePtr  mPipelineState;

    d3d12::ID3D12RootSignaturePtr mRootSignature;
};

}  // namespace sample