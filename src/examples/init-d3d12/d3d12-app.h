#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <stdint.h>
#include <wrl.h>
#include <vector>

#include "app-framework/application.h"
#include "app-framework/input.h"

namespace d3d12_sandbox {

using namespace Microsoft::WRL;

using physika::Keycode;
using physika::MouseButton;

class D3D12App : public physika::Application
{
public:
    D3D12App(TCHAR const* const title, int width, int height);
    bool Initialize() override;
    bool Shutdown() override;
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
    bool CreateSwapChain();
    bool CreateDescriptorHeaps();
    bool CreateRenderTargetView();

    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

    int mSwapChainBufferCount;
    int mCurrentBackBuffer;

    uint32_t    mCbvSrvDescriptorSize;
    uint32_t    mDsvDescriptorSize;
    uint32_t    mRtvDescriptorSize;
    DXGI_FORMAT mBackBufferFormat;

    ComPtr<IDXGIFactory>                mFactory;
    ComPtr<IDXGISwapChain>              mSwapChain;
    std::vector<ComPtr<ID3D12Resource>> mSwapChainBuffers;

    ComPtr<ID3D12Device>              mDevice;
    ComPtr<ID3D12Fence>               mFence;
    ComPtr<ID3D12CommandQueue>        mCommandQueue;
    ComPtr<ID3D12CommandAllocator>    mCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;
    ComPtr<ID3D12Debug>               mDebugController;

    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;
};

}  // namespace d3d12_sandbox
