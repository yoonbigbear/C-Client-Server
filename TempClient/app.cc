#include "app.h"
#include "gui.h"
void App::Initialize()
{
    graphics_.Initialize();
    nav_mesh_.Start("../Resource/all_tiles_navmesh.bin");
}

void App::Run()
{
    if (!nav_mesh_.nav_mesh())
        return;

    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_PollEvent(&event);
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE
            && event.window.windowID == SDL_GetWindowID(graphics_.window()))
            done = true;

        graphics_.BeginScene();
        graphics_.Input(&event);

        nav_mesh_.Render();
        Gui::GuiList();
        client_.GuiLogin();
        agent_.drawAgent(dd, agent_.pos, agent_.radius_,
            agent_.height_, agent_.max_climb, startCol);
        graphics_.EndScene();
    }
}
