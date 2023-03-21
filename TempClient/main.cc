#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "app.h"

int main(int, char**)
{
    //ShowWindow(GetConsoleWindow(), SW_HIDE);

    auto app = std::make_unique<App>();
    app->Initialize();

    app->Run();

    app.release();

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return 0;
}
