
#include <stdio.h>

#include "d3d11-basic.h"
#include "logger/logger.h"

int main()
{
    sample::D3D11Basic app(_T("D3D11 Basic"), 1920, 1080);

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
