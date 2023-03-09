#include "app.h"

int main(int, char**)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    auto app = std::make_unique<App>();
    app->Initialize();

    app->Run();

    app.release();

    return 0;
}
