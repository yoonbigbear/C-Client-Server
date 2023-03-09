#include "level.h"

#include <gui.h>

void Level::Initialize(const char* path)
{
    nav_mesh_.Initialize(path);

    Gui::instance().level_info.onUpdateList = [this](int selected) {
        size_t size = agents_.size();
        for (int i = 0; i < size; i++)
        {
            // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
            char label[128];
            sprintf_s(label, "MyObject %d", i);
            if (ImGui::Selectable(label, selected == i))
            {
                selected = i;
            }
        }
    };
}

void Level::Draw()
{
    nav_mesh_.Render();
    for (auto& e : agents_)
    {
        e.drawAgent(e.pos, e.radius_,
            e.height_, e.max_climb, startCol);
    }
}
