
#include <stdio.h>

#include "d3d12-shapes.h"
#include "logger/logger.h"

int main()
{
    sample::D3D12Shapes app(_T("D3D12 Basic"), 1920, 1080);

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
