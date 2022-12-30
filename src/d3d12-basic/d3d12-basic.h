#pragma once

#include <d3d12.h>
#include <dxgi1_3.h>
#include <stdint.h>      // uint32_t
#include <wrl/client.h>  // ComPtr

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

    uint32_t    mSwapChainBufferCount;
    DXGI_FORMAT mBackBufferFormat;

    // Device Objects
    WRL::ComPtr<ID3D12Device>  mD3D12Device;
    WRL::ComPtr<IDXGIFactory1> mDXGIFactory;
    WRL::ComPtr<IDXGIAdapter1> mDXGIAdapter;

    WRL::ComPtr<ID3D12Debug1>   mD3D12DebugController;
    WRL::ComPtr<IDXGISwapChain> mSwapChain;
};

}  // namespace sample
