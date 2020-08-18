#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <stdint.h>
#include <wrl.h>

#include <queue>
#include <vector>

#include "app-framework/application.h"
#include "app-framework/input.h"
#include "timer/timer.h"

namespace d3d12_sandbox {

using namespace Microsoft::WRL;

using physika::Keycode;
using physika::MouseButton;

class D3D12App : public physika::Application
{
public:
    D3D12App(TCHAR const* const title, int width, int height);
    ~D3D12App();
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
    bool CreateGraphicsDevice();
    bool EnumerateAdapters();
    void QueryDeviceProperties();
    bool CreateCommandObjects();
    bool CreateFence();
    bool CreateSwapChain();
    bool CreateDescriptorHeaps();
    bool CreateRenderTargetView();
    bool CreateDepthStencilBufferAndView();
    void CalculateFrameStatistics();
    void ResizeViewportAndScissorRect();

    ID3D12Resource*             CurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

    void FlushCommandQueue();
    void Update();
    void Draw();

    physika::Timer    mTimer;
    std::queue<float> mFrameTimes;

    uint32_t mSwapChainBufferCount;
    uint32_t mCurrentBackBuffer;

    uint32_t    mCbvSrvDescriptorSize;
    uint32_t    mDsvDescriptorSize;
    uint32_t    mRtvDescriptorSize;
    DXGI_FORMAT mBackBufferFormat;

    D3D12_VIEWPORT mViewport;
    D3D12_RECT     mScissorRect;

    ComPtr<IDXGIFactory>                mFactory;
    ComPtr<IDXGISwapChain>              mSwapChain;
    std::vector<ComPtr<ID3D12Resource>> mSwapChainBuffers;
    ComPtr<ID3D12Resource>              mDepthStencilBuffer;

    uint64_t mFenceValue;

    ComPtr<ID3D12Device>              mDevice;
    ComPtr<ID3D12Fence>               mFence;
    ComPtr<ID3D12CommandQueue>        mCommandQueue;
    ComPtr<ID3D12CommandAllocator>    mCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;
    ComPtr<ID3D12Debug>               mDebugController;

    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS mMSAAQualityLevels;
};

}  // namespace d3d12_sandbox
