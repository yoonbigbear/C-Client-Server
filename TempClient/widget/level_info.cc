#include "level_info.h"

#include <stdio.h>

#include "imgui.h"
#include "../manager/level_manager.h"

void LevelInfo::Draw()
{
    bool open = true;

    ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Level Infomation", &open))
    {
        auto level = LevelManager::instance().Get(level_id);
        // Left
        static int selected = 0;
        auto agents = level->agents();
        auto size = agents.size();
        {
            ImGui::BeginChild("object list", ImVec2(150, 0), true);
 
            onUpdateList(selected);
            //for (int i = 0; i < size; i++)
            //{
            //    // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
            //    char label[128];
            //    sprintf_s(label, "MyObject %d", i);
            //    if (ImGui::Selectable(label, selected == i))
            //    {
            //        selected = i;
            //    }
            //}
            ImGui::EndChild();
        }
        ImGui::SameLine();

        // Right
        if (size > 0)
        {
            ImGui::BeginGroup();
            ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
            ImGui::Text("MyObject: %d", selected);
            ImGui::Separator();
            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Description"))
                {
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Details"))
                {
                    ImGui::Text("ID: 0123456789");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}
