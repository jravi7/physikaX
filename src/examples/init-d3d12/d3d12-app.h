#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "app-framework/application.h"
#include "app-framework/input.h"

namespace d3d12_sandbox {

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
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
};

}  // namespace d3d12_sandbox
