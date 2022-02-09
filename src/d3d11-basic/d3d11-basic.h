#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <stdint.h>      // uint32_t
#include <wrl/client.h>  // ComPtr

#include "app-framework/application.h"
#include "app-framework/input.h"

namespace sample {

using physika::Keycode;
using physika::MouseButton;

class D3D11Basic : public physika::Application
{
public:
    D3D11Basic(TCHAR const* const title, int width, int height);
    ~D3D11Basic();

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
    bool EnumerateAdapters();
    bool CreateGraphicsDevice();
    bool CreateSwapChain();
    bool CreateRenderTargetView();
    bool CreateDepthStencilView();
    bool CreateDepthStencilState();
    bool SetupRasterStateAndViewport();

    void Render();

    uint32_t    mSwapChainBufferCount;
    DXGI_FORMAT mBackBufferFormat;

    Microsoft::WRL::ComPtr<ID3D11Device>            mDevice;
    Microsoft::WRL::ComPtr<IDXGIFactory>            mFactory;
    Microsoft::WRL::ComPtr<IDXGISwapChain>          mSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     mDeviceContext;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>         mDepthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  mDepthStencilView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  mRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>   mRasterState;
};

}  // namespace sample
