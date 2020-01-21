/**
Apr 8, 2018
Jay R Ravi
**/

#include <stdio.h>
#include <string>
#include "d3d12-app.h"

using namespace d3d12_sandbox;

std::string GetLastErrorAsString()
{
    // Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string();  // No error message has been recorded

    LPSTR  messageBuffer = nullptr;
    size_t size          = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    // Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

int main()
{
    D3D12App app(_T("Init D3D12"), 1920, 1080);

    if (!app.Initialize()) {
        printf("Could not initialize app. Exiting");
        return 1;
    }
    app.Run();

    if (!app.Shutdown()) {
        printf("%s\n", GetLastErrorAsString().c_str());
        printf("Shutdown sequence failed.");
        return 1;
    }
    return 0;
}
