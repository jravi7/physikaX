#include "example-app.h"

#include <stdio.h>

namespace example {

ExampleApp::ExampleApp(TCHAR const* const title, int width, int height)
    : physika::Application(title, width, height)
{
}

void ExampleApp::OnUpdate()
{
}

void ExampleApp::OnResize(int /*width*/, int /*height*/)
{
}

void ExampleApp::OnKeyUp(Keycode /*key*/)
{
}

void ExampleApp::OnKeyDown(Keycode /*key*/)
{
}

void ExampleApp::OnMouseUp(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void ExampleApp::OnMouseDown(MouseButton /*button*/, int /*x*/, int /*y*/)
{
}

void ExampleApp::OnMouseMove(int /*x*/, int /*y*/)
{
}

void ExampleApp::OnMouseWheel(int /*delta*/)
{
}

}  // namespace example