/**
May 17, 2020
Jay R Ravi
**/

#include "d3d12-app.h"
#include "logger/logger.h"

using namespace physika::logger;
using namespace d3d12_sandbox;

int main()
{
    SetApplicationName("Init D3D12");
    SetLoggingLevel(LogLevel::kInfo);

    D3D12App app(_T("Init D3D12"), 1920, 1080);

    if (!app.Initialize()) {
        LOG_FATAL("Could not initialize app. Exiting");
        return 1;
    }
    app.Run();

    if (!app.Shutdown()) {
        LOG_FATAL("Shutdown sequence failed.");
        return 1;
    }
    return 0;
}
