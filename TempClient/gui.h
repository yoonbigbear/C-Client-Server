#pragma once

#include "imgui.h"
#include "singleton.h"

#include "widget/login_window.h"
#include "widget/scene_info.h"
#include "widget/log_widget.h"
#include "widget/console_widget.h"


class Gui : public Singleton<Gui>
{
public:
    static void GuiList()
    {
        ImGui::ShowDemoWindow();
        //// Our state
        //bool show_demo_window = true;
        //bool show_another_window = false;
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
             //ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        //{
        //    static float f = 0.0f;
        //    static int counter = 0;

        //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //    //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //    //ImGui::Checkbox("Another Window", &show_another_window);

        //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //    //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //        counter++;
        //    ImGui::SameLine();
        //    ImGui::Text("counter = %d", counter);

        //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //    ImGui::End();
        //}
    }
    void Draw();

public:
    LogWidget log;
    ConsoleWidget console;
    LoginWindow login;
    SceneInfo level_info;
};

//static ExampleAppConsole console;
//console.Draw("Example: Console", p_open);