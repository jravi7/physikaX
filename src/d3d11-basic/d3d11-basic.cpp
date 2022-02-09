#include "d3d11-basic.h"

#include <stdio.h>

#include <string>

#include "logger/logger.h"

namespace {

void PrintHeader(char const* message)
{
    size_t const headerSize = strnlen(message, 1024);
    std::string  decoration(headerSize, '-');
    printf("%s\n", decoration.c_str());
    printf("%s\n", message);
    printf("%s\n", decoration.c_str());
}

}  // namespace

namespace sample {

using namespace physika;

D3D11Basic::D3D11Basic(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height),
      mBackBufferFormat{ DXGI_FORMAT_R8G8B8A8_UNORM },
      mSwapChainBufferCount{ 2 }
{
}

D3D11Basic::~D3D11Basic()
{
}

bool D3D11Basic::Initialize()
{
    if (!physika::Application::Initialize()) {
        return false;
    }

    logger::SetApplicationName("D3D11 Basic");

    EnumerateAdapters();

    if (!CreateGraphicsDevice()) {
        return false;
    }

    if (!CreateSwapChain()) {
        return false;
    }

    if (!CreateRenderTargetView()) {
        return false;
    }

    if (!CreateDepthStencilView()) {
        return false;
    }

    if (!CreateDepthStencilState()) {
        return false;
    }

    if (!SetupRasterStateAndViewport()) {
        return false;
    }

    return true;
}

bool D3D11Basic::Shutdown()
{
    if (!physika::Application::Shutdown()) {
        return false;
    }
    return true;
}

bool D3D11Basic::EnumerateAdapters()
{
    PrintHeader("Enumerating adapters");

    IDXGIAdapter* pAdapter = nullptr;
    if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&mFactory)))) {
        logger::LOG_FATAL("Failed to create a DXGI Factory instance");
        return false;
    }
    for (uint32_t i = 0;
         mFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC desc{};
        pAdapter->GetDesc(&desc);

        logger::LOG_INFO("%s", desc.Description);
    }
    return true;
}

bool D3D11Basic::CreateGraphicsDevice()
{
    EnumerateAdapters();

    PrintHeader("Setting up D3D11 Device");

    const D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_1;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   0, &featureLevels, 1, D3D11_SDK_VERSION,
                                   mDevice.ReleaseAndGetAddressOf(), nullptr,
                                   mDeviceContext.ReleaseAndGetAddressOf());

    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create D3D11 Device and Device Context");
        return false;
    }

    logger::LOG_DEBUG(
        "D3D11 Device and Device Context created successfully created.");

    return true;
}

bool D3D11Basic::CreateSwapChain()
{
    PrintHeader("Setting up SwapChain");

    mSwapChain.Reset();
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    // Back buffer description
    swapChainDesc.BufferDesc.Width                   = mWindowWidth;
    swapChainDesc.BufferDesc.Height                  = mWindowHeight;
    swapChainDesc.BufferDesc.Format                  = mBackBufferFormat;
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering =
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    //! Sample Desc
    swapChainDesc.SampleDesc.Count   = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    //! Other properties
    swapChainDesc.Windowed     = true;
    swapChainDesc.BufferCount  = mSwapChainBufferCount;
    swapChainDesc.OutputWindow = mHwnd;
    swapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.Flags        = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr =
        mFactory->CreateSwapChain(mDevice.Get(), &swapChainDesc, &mSwapChain);
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create a DXGI SwapChain");
        return false;
    }

    logger::LOG_DEBUG("Swap chain successfully created.");

    return true;
}

bool D3D11Basic::CreateRenderTargetView()
{
    PrintHeader("Setting up Render Target View");
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    HRESULT hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to get swapchain backbuffer.");
        return false;
    }

    hr = mDevice->CreateRenderTargetView(
        pBackBuffer.Get(), nullptr, mRenderTargetView.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create render target view.");
        return false;
    }
    logger::LOG_DEBUG("Render Target View successfully created.");
    return true;
}

bool D3D11Basic::CreateDepthStencilView()
{
    PrintHeader("Setting up Depth Stencil View");
    D3D11_TEXTURE2D_DESC desc{};

    desc.Width              = mWindowWidth;
    desc.Height             = mWindowHeight;
    desc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.ArraySize          = 1;
    desc.MipLevels          = 1;
    desc.CPUAccessFlags     = 0;
    desc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    desc.Usage              = D3D11_USAGE_DEFAULT;
    desc.MiscFlags          = 0;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    // Create the texture for the depth buffer using the filled out description.
    auto hr = mDevice->CreateTexture2D(&desc, NULL, &mDepthStencilBuffer);
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create depth stencil buffer.");
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
    viewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MipSlice = 0;
    viewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;

    hr = mDevice->CreateDepthStencilView(
        mDepthStencilBuffer.Get(), &viewDesc,
        mDepthStencilView.ReleaseAndGetAddressOf());

    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create depth stencil view.");
        return false;
    }

    logger::LOG_DEBUG("Depth Stencil View successfully created.");

    return true;
}
bool D3D11Basic::CreateDepthStencilState()
{
    PrintHeader("Setting up Depth Stencil State");

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable              = true;
    depthStencilDesc.DepthWriteMask           = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc                = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable    = true;
    depthStencilDesc.StencilReadMask  = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

    auto hr = mDevice->CreateDepthStencilState(
        &depthStencilDesc, mDepthStencilState.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create depth stencil state.");
        return false;
    }
    mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);

    logger::LOG_DEBUG("Depth Stencil State successfully created.");
    return true;
}

bool D3D11Basic::SetupRasterStateAndViewport()
{
    PrintHeader("Setting up rasterizer state and viewport.");

    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode              = D3D11_CULL_BACK;
    rasterDesc.DepthBias             = 0;
    rasterDesc.DepthBiasClamp        = 0.0f;
    rasterDesc.DepthClipEnable       = true;
    rasterDesc.FillMode              = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable     = false;
    rasterDesc.ScissorEnable         = false;
    rasterDesc.SlopeScaledDepthBias  = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    auto hr = mDevice->CreateRasterizerState(
        &rasterDesc, mRasterState.ReleaseAndGetAddressOf());
    if (FAILED(hr)) {
        logger::LOG_FATAL("Failed to create rasterizer state.");
        return false;
    }

    // Now set the rasterizer state.
    mDeviceContext->RSSetState(mRasterState.Get());

    D3D11_VIEWPORT viewport;
    viewport.Width    = (float)mWindowWidth;
    viewport.Height   = (float)mWindowHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    mDeviceContext->RSSetViewports(1, &viewport);

    logger::LOG_DEBUG("Raster State and Viewport successfully setup.");
    return true;
}

void D3D11Basic::OnUpdate()
{
    Render();
}

void D3D11Basic::Render()
{
    mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(),
                                       mDepthStencilView.Get());

    float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

    // Clear the back buffer.
    mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), color);

    // Clear the depth buffer.
    mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(),
                                          D3D11_CLEAR_DEPTH, 1.0f, 0);

    mSwapChain->Present(0, 0);
}

void D3D11Basic::OnResize(int /*width*/, int /*height*/)
{
}

void D3D11Basic::OnKeyUp(Keycode /*key*/)
{
}

void D3D11Basic::OnKeyDown(Keycode /*key*/)
{
}

void D3D11Basic::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D11Basic::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D11Basic::OnMouseMove(int /*x*/, int /*y*/)
{
}

void D3D11Basic::OnMouseWheel(int /*delta*/)
{
}

}  // namespace sample