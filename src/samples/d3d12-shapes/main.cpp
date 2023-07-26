
#include <stdio.h>

#include "d3d12-shapes.h"
#include "logger/logger.h"

int main()
{
    float               aspectRatio43 = 4.0f / 3.0f;
    int                 windowWidth   = 1024;
    int                 windowHeight  = static_cast<int>(windowWidth / aspectRatio43);
    sample::D3D12Shapes app(_T("D3D12 Shapes"), windowWidth, windowHeight);

    if (!app.Initialize()) {
        printf("Could not initialize app. Exiting");
        return 1;
    }
    app.Run();

    if (!app.Shutdown()) {
        printf("Shutdown sequence failed.");
        return 1;
    }
    return 0;
}
