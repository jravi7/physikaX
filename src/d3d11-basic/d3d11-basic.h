#pragma once

#include <d3d11.h>
#include <dxgi.h>
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
    Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
};

}  // namespace sample
