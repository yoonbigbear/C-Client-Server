#include "src/server.h"

int main()
{
    Server server(11000);
    server.Start();
    while (true)
    {
        server.Update();
    }
    return 0;
}