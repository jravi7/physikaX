#include <iostream>

#include "d3d12-app.h"

namespace d3d12_sandbox {

D3D12App::D3D12App(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height)
{
}

bool D3D12App::Initialize()
{
    if (!physika::Application::Initialize()) {
        return false;
    }

    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0,
                                 IID_PPV_ARGS(&mDevice)))) {
        return false;
    }

    std::cout << "Graphics Device Initialized" << std::endl;

    return true;
}
bool D3D12App::Shutdown()
{
    if (!physika::Application::Shutdown()) {
        return false;
    }
    return true;
}

void D3D12App::OnUpdate()
{
}

void D3D12App::OnResize(int /*width*/, int /*height*/)
{
}

void D3D12App::OnKeyUp(Keycode /*key*/)
{
}

void D3D12App::OnKeyDown(Keycode /*key*/)
{
}

void D3D12App::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12App::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void D3D12App::OnMouseMove(int /*x*/, int /*y*/)
{
}

void D3D12App::OnMouseWheel(int /*delta*/)
{
}

}  // namespace d3d12_sandbox