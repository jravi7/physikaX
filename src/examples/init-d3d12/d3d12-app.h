#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>

#include "app-framework/application.h"
#include "app-framework/input.h"

namespace d3d12_sandbox {

using namespace Microsoft;

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
    bool EnumerateAdapters();
    bool CreateCommandObjects();
    bool CreateSwapChain();

    uint32_t mCbvSrvDescriptorSize;
    uint32_t mDsvDescriptorSize;
    uint32_t mRtvDescriptorSize;

    uint32_t mSwapChainBufferCount;

    WRL::ComPtr<IDXGIFactory>   mFactory;
    WRL::ComPtr<IDXGISwapChain> mSwapChain;

    WRL::ComPtr<ID3D12Device>              mDevice;
    WRL::ComPtr<ID3D12Fence>               mFence;
    WRL::ComPtr<ID3D12CommandQueue>        mCommandQueue;
    WRL::ComPtr<ID3D12CommandAllocator>    mCommandAllocator;
    WRL::ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;
    WRL::ComPtr<ID3D12Debug>               mDebugController;

    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
};

}  // namespace d3d12_sandbox
