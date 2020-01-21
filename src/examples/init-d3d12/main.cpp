/**
Apr 8, 2018
Jay R Ravi
**/

#include <iostream>
#include "d3d12-app.h"

using namespace d3d12_sandbox;

int main()
{
    D3D12App app(_T("Init D3D12"), 1920, 1080);

    if (!app.Initialize()) {
        std::cout << "Could not initialize app. Exiting" << std::endl;
        return 1;
    }
    app.Run();

    if (!app.Shutdown()) {
        std::cout << "Shutdown sequence failed.";
        return 1;
    }
    return 0;
}
