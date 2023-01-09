#pragma once

#include <d3d12.h>
#include <dxgi1_3.h>
#include <stdint.h>      // uint32_t
#include <wrl/client.h>  // ComPtr

#include <memory>  // unique_ptr
#include <vector>

#include "app-framework/application.h"
#include "app-framework/input.h"
#include "d3d12-util.h"

namespace sample {

using physika::Keycode;
using physika::MouseButton;
using namespace Microsoft;

class D3D12Basic : public physika::Application
{
public:
    D3D12Basic(TCHAR const* const title, int width, int height);
    ~D3D12Basic();

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

    void FlushCommandQueue();
    void Update();
    void Draw();

    uint32_t    mSwapChainBufferCount;
    uint32_t    mCurrentBackBuffer;
    DXGI_FORMAT mBackBufferFormat;

    // Device Objects
    WRL::ComPtr<ID3D12Device>              mD3D12Device;
    WRL::ComPtr<IDXGIFactory1>             mDXGIFactory;
    WRL::ComPtr<IDXGIAdapter1>             mDXGIAdapter;
    WRL::ComPtr<ID3D12CommandQueue>        mCommandQueue;
    WRL::ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;
    WRL::ComPtr<ID3D12CommandAllocator>    mCommandAllocator;
    WRL::ComPtr<ID3D12Debug1>              mD3D12DebugController;
    WRL::ComPtr<ID3D12DescriptorHeap>      mRtvHeap;
    WRL::ComPtr<ID3D12DescriptorHeap>      mDsvHeap;
    WRL::ComPtr<ID3D12DescriptorHeap>      mSRVHeap;

    //! Window surface objects
    WRL::ComPtr<IDXGISwapChain>              mSwapChain;
    std::vector<WRL::ComPtr<ID3D12Resource>> mSwapChainBackBuffers;
    WRL::ComPtr<ID3D12Resource>              mDepthStencilBuffer;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    //! Sync Variables
    uint64_t                 mFenceValue;
    HANDLE                   mFenceEventHandle;
    WRL::ComPtr<ID3D12Fence> mFence;

    //! Resources
    std::unique_ptr<physika::d3d12_util::Mesh> mMeshBuffers;
    WRL::ComPtr<ID3D12PipelineState>           mPipelineState;
    WRL::ComPtr<ID3D12RootSignature>           mRootSignature;
};

}  // namespace sample
