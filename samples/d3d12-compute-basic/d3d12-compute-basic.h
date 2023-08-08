#pragma once

#include <d3d12.h>
#include <dxgi1_3.h>
#include <stdint.h>      // uint32_t
#include <wrl/client.h>  // ComPtr

#include <memory>  // unique_ptr
#include <vector>

#include "core/application.h"
#include "core/input.h"
#include "graphics/helpers.h"
#include "graphics/types.h"

namespace sample {

using namespace physika;
using physika::core::Keycode;
using physika::core::MouseButton;

class D3D12ComputeBasic : public physika::core::Application
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
    graphics::ID3D12DevicePtr              mD3D12Device;
    graphics::IDXGIFactory1Ptr             mDXGIFactory;
    graphics::IDXGIAdapter1Ptr             mDXGIAdapter;
    graphics::ID3D12CommandQueuePtr        mCommandQueue;
    graphics::ID3D12GraphicsCommandListPtr mGraphicsCommandList;
    graphics::ID3D12CommandAllocatorPtr    mCommandAllocator;
    graphics::ID3D12Debug1Ptr              mD3D12DebugController;
    graphics::ID3D12DescriptorHeapPtr      mRtvHeap;
    graphics::ID3D12DescriptorHeapPtr      mDsvHeap;
    graphics::ID3D12DescriptorHeapPtr      mSRVHeap;

    //! Window surface objects
    graphics::IDXGISwapChainPtr              mSwapChain;
    std::vector<graphics::ID3D12ResourcePtr> mSwapChainBackBuffers;
    graphics::ID3D12ResourcePtr              mDepthStencilBuffer;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    //! Sync Variables
    uint64_t                 mFenceValue;
    HANDLE                   mFenceEventHandle;
    graphics::ID3D12FencePtr mFence;

    //! Resources
    graphics::ID3D12DescriptorHeapPtr mUAVHeap;
    graphics::DefaultGPUBuffer        mVectorOfNumbers1;
    graphics::DefaultGPUBuffer        mVectorOfNumbers2;
    graphics::OutputBuffer            mVectorAddResult;
    graphics::ID3D12ResourcePtr       mResultBuffer;
    graphics::ID3D12PipelineStatePtr  mPipelineState;

    graphics::ID3D12RootSignaturePtr mRootSignature;
};

}  // namespace sample
