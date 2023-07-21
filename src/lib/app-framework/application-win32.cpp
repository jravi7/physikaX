
#include "app-framework/application-win32.h"

#include <assert.h>
#include <crtdbg.h>
#include <stdio.h>
#include <windows.h>

namespace {
using namespace physika;

ApplicationWin32* sApp = nullptr;

MouseButton MapMouseWin32ToPhi(WPARAM wParam)
{
    MouseButton result = kMouseNone;
    if (wParam & MK_LBUTTON) {
        result |= kMouseLeft;
    }
    if (wParam & MK_RBUTTON) {
        result |= kMouseRight;
    }
    if (wParam & MK_MBUTTON) {
        result |= kMouseMiddle;
    }
    return result;
}

Keycode MapKeyWin32ToPhi(WPARAM wParam)
{
    switch (wParam) {
    case 0x41:
        return kA;
    case 0x42:
        return kB;
    case 0x43:
        return kC;
    case 0x44:
        return kD;
    case 0x45:
        return kE;
    case 0x46:
        return kF;
    case 0x47:
        return kG;
    case 0x48:
        return kH;
    case 0x49:
        return kI;
    case 0x4A:
        return kJ;
    case 0x4B:
        return kK;
    case 0x4C:
        return kL;
    case 0x4D:
        return kM;
    case 0x4E:
        return kN;
    case 0x4F:
        return kO;
    case 0x50:
        return kP;
    case 0x51:
        return kQ;
    case 0x52:
        return kR;
    case 0x53:
        return kS;
    case 0x54:
        return kT;
    case 0x55:
        return kU;
    case 0x56:
        return kV;
    case 0x57:
        return kW;
    case 0x58:
        return kX;
    case 0x59:
        return kY;
    case 0x5A:
        return kZ;
    case 0x30:
        return k0;
    case 0x31:
        return k1;
    case 0x32:
        return k2;
    case 0x33:
        return k3;
    case 0x34:
        return k4;
    case 0x35:
        return k5;
    case 0x36:
        return k6;
    case 0x37:
        return k7;
    case 0x38:
        return k8;
    case 0x39:
        return k9;
    case VK_NUMPAD0:
        return kNumpad0;
    case VK_NUMPAD1:
        return kNumpad1;
    case VK_NUMPAD2:
        return kNumpad2;
    case VK_NUMPAD3:
        return kNumpad3;
    case VK_NUMPAD4:
        return kNumpad4;
    case VK_NUMPAD5:
        return kNumpad5;
    case VK_NUMPAD6:
        return kNumpad6;
    case VK_NUMPAD7:
        return kNumpad7;
    case VK_NUMPAD8:
        return kNumpad8;
    case VK_NUMPAD9:
        return kNumpad9;
    case VK_MULTIPLY:
        return kMultiply;
    case VK_ADD:
        return kAdd;
    case VK_SEPARATOR:
        return kSeparator;
    case VK_SUBTRACT:
        return kSubtract;
    case VK_DECIMAL:
        return kDecimal;
    case VK_DIVIDE:
        return kDivide;
    case VK_F1:
        return kF1;
    case VK_F2:
        return kF2;
    case VK_F3:
        return kF3;
    case VK_F4:
        return kF4;
    case VK_F5:
        return kF5;
    case VK_F6:
        return kF6;
    case VK_F7:
        return kF7;
    case VK_F8:
        return kF8;
    case VK_F9:
        return kF9;
    case VK_F10:
        return kF10;
    case VK_F11:
        return kF11;
    case VK_F12:
        return kF12;
    case VK_LSHIFT:
        return kLeftShift;
    case VK_RSHIFT:
        return kRightShift;
    case VK_LCONTROL:
        return kLeftControl;
    case VK_RCONTROL:
        return kRightControl;
    case VK_LMENU:
        return kLeftAlt;
    case VK_RMENU:
        return kRightAlt;
    case VK_LWIN:
        return kLeftSuper;
    case VK_RWIN:
        return kRightSuper;
    case VK_SPACE:
        return kSpace;
    case VK_ESCAPE:
        return kEsc;
    default:
        return kKeyCodeCount;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int x      = 0;
    int y      = 0;
    int width  = 0;
    int height = 0;
    int delta  = 0;

    MouseButton mouseButton = MouseButton::kMouseNone;
    switch (message) {
    case WM_PAINT:
        break;
    case WM_KEYUP:
        sApp->OnKeyUp(MapKeyWin32ToPhi(wParam));
        break;
    case WM_KEYDOWN:
        sApp->OnKeyDown(MapKeyWin32ToPhi(wParam));
        break;
    case WM_CHAR:
        break;
    case WM_LBUTTONDOWN:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouseButton |= kMouseLeft;
        mouseButton |= MapMouseWin32ToPhi(wParam);
        sApp->OnMouseDown(mouseButton, x, y);
        break;
    case WM_MBUTTONDOWN:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouseButton |= kMouseMiddle;
        mouseButton |= MapMouseWin32ToPhi(wParam);
        sApp->OnMouseDown(mouseButton, x, y);
        break;
    case WM_RBUTTONDOWN:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouseButton |= kMouseRight;
        mouseButton |= MapMouseWin32ToPhi(wParam);
        sApp->OnMouseDown(mouseButton, x, y);
        break;
    case WM_LBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouseButton |= kMouseLeft;
        mouseButton |= MapMouseWin32ToPhi(wParam);
        sApp->OnMouseUp(mouseButton, x, y);
        break;
    case WM_MBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouseButton |= kMouseMiddle;
        mouseButton |= MapMouseWin32ToPhi(wParam);
        sApp->OnMouseUp(mouseButton, x, y);
        break;
    case WM_RBUTTONUP:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        mouseButton |= kMouseRight;
        mouseButton |= MapMouseWin32ToPhi(wParam);
        sApp->OnMouseUp(mouseButton, x, y);
        break;
    case WM_MOUSEMOVE:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        sApp->OnMouseMove(x, y);
        break;
    case WM_MOUSEWHEEL:
        delta = GET_WHEEL_DELTA_WPARAM(wParam);
        sApp->OnMouseWheel(delta);
        break;
    case WM_SIZE:
        width  = LOWORD(lParam);
        height = HIWORD(lParam);
        sApp->OnResize(width, height);
        break;
    case WM_DESTROY: {
        assert(DestroyWindow(hWnd) && "Failed to destroy window successfully");
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return LRESULT();
}

}  // namespace

using namespace physika;

ApplicationWin32::ApplicationWin32(TCHAR const* const title, int width, int height)
    : mWindowTitle(title),
      mWindowWidth(width),
      mWindowHeight(height),
      mHinstance{ nullptr },
      mHwnd{ nullptr }
{
}

bool ApplicationWin32::Initialize()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    sApp                  = this;
    mHinstance            = GetModuleHandle(nullptr);
    TCHAR szWindowClass[] = _T("win32app");

    WNDCLASSEX wcex;
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc   = WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = mHinstance;
    wcex.hIcon         = nullptr;
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm       = nullptr;

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("Call to RegisterClassEx failed"), _T("Error"), NULL);
        return false;
    }

    mHwnd = CreateWindow(szWindowClass, mWindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                         CW_USEDEFAULT, mWindowWidth, mWindowHeight, NULL, NULL, mHinstance, NULL);

    if (!mHwnd) {
        MessageBox(NULL, _T("Window Creation failed"), _T("Error"), NULL);
        return 1;
    }

    ShowWindow(mHwnd, SW_SHOW);
    UpdateWindow(mHwnd);

    return true;
}

bool ApplicationWin32::Shutdown()
{
    //! Derived classes can add logic to control game shutdown sequence
    return true;
}

void* ApplicationWin32::ApplicationHandle()
{
    return mHinstance;
}

void* ApplicationWin32::WindowHandle()
{
    return mHwnd;
}

void ApplicationWin32::Run()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (1) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_QUIT) {
            break;
        } else {
            OnUpdate();
        }
    }
}

void ApplicationWin32::OnUpdate()
{
}

void ApplicationWin32::OnResize(int /*width*/, int /*height*/)
{
}

void ApplicationWin32::OnKeyUp(Keycode /*key*/)
{
}

void ApplicationWin32::OnKeyDown(Keycode /*key*/)
{
}

void ApplicationWin32::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void ApplicationWin32::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void ApplicationWin32::OnMouseMove(int /*x*/, int /*y*/)
{
}

void ApplicationWin32::OnMouseWheel(int /*delta*/)
{
}
