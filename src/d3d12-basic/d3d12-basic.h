#pragma once

#include <d3d12.h>
#include <dxgi1_3.h>
#include <stdint.h>      // uint32_t
#include <wrl/client.h>  // ComPtr

#include <vector>

#include "app-framework/application.h"
#include "app-framework/input.h"

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
    bool InitializeDeviceFactory();
    bool InitializeGraphicsDevice();
    bool InitializeSwapChain();
    bool InitializeCommandObjects();
    bool InitializeFenceObjects();

    bool CreateDescriptorHeaps();
    bool CreateRenderTargetView();
    bool CreateDepthStencilBufferAndView();
    // void CalculateFrameStatistics();
    void ResizeViewportAndScissorRect();

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

    //! Window surface objects
    WRL::ComPtr<IDXGISwapChain>              mSwapChain;
    std::vector<WRL::ComPtr<ID3D12Resource>> mSwapChainBackBuffers;
    WRL::ComPtr<ID3D12Resource>              mDepthStencilBuffer;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    //! Sync Variables
    uint64_t                 mFenceValue;
    WRL::ComPtr<ID3D12Fence> mFence;
};

}  // namespace sample
