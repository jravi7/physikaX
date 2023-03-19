/**
Apr 8, 2018
Jay R Ravi
**/

#include <stdio.h>

#include "example-app.h"

int main()
{
    example::ExampleApp app(_T("Physika App Framework"), 1024, 768);

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
