#pragma once

#include <Windows.h>
#include <tchar.h>

#include "app-framework/input.h"

namespace physika {

//! @brief  A Win32 application class that handles window
//!         creation and OS message handling. Inherit from
//!         this class to add custom logic to callbacks
class ApplicationWin32
{
public:
    ApplicationWin32(TCHAR const* const title, int width, int height);

    //! @brief Call this to initialize application.
    //! @return Bool value indicating success or failure
    virtual bool Initialize();

    //! @brief Call this to shutdown application.
    //! @return Bool value indicating success or failure
    virtual bool Shutdown();

    //! @brief Call this to begin event loop.
    void Run();

    //! @brief Override this callback to handle frame updates.
    virtual void OnUpdate();

    //! @brief Override this to handle window resizing.
    virtual void OnResize(int width, int height);

    //! @brief Override this to handle key up event.
    virtual void OnKeyUp(Keycode key);

    //! @brief Override this to handle key down event.
    virtual void OnKeyDown(Keycode key);

    //! @brief Override this to handle mouse up event.
    virtual void OnMouseUp(MouseButton button, int x, int y);

    //! @brief Override this to handle mouse down event.
    virtual void OnMouseDown(MouseButton button, int x, int y);

    //! @brief Override this to handle mouse movement event.
    virtual void OnMouseMove(int x, int y);

    //! @brief Override this to handle mouse wheel rotation event.
    virtual void OnMouseWheel(int delta);

    //! @brief Returns App Handle as a void*
    void* ApplicationHandle();

    //! @brief Returns window handle as a void*
    void* WindowHandle();

protected:
    int          mWidth;
    int          mHeight;
    TCHAR const* mWindowTitle;

private:
    //! OS Specific
    HWND      mHwnd;
    HINSTANCE mHinstance;
};

}  // namespace physika
