#include "d3d11-basic.h"

#include <stdio.h>

namespace sample {

D3D11Basic::D3D11Basic(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height)
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
    return true;
}

bool D3D11Basic::Shutdown()
{
    if (!physika::Application::Shutdown()) {
        return false;
    }
    return true;
}

void D3D11Basic::OnUpdate()
{
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