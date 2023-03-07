#include "app.h"



int main(int, char**)
{
    auto app = std::make_unique<App>();
    app->Initialize();

    app->Run();

    app.release();

    return 0;
}
