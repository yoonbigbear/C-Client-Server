#include "login_window.h"

#include "imgui.h"

void LoginWindow::Draw()
{
    if (!login)
    {
        ImGui::Begin("Login");

        const char* ip = "127.0.0.1";
        [[maybe_unused]]const char* ppw = "";
        uint16_t port = 11000;

        if (ImGui::Button("Connect"))
        {
            onClickConnect(ip, port);
        }

        ImGui::Separator();
        ImGui::Text("render fps: %d / simulate fps: %d", render_frame, simulate_frame);
        ImGui::End();
    }
    else
    {
        ImGui::Begin("Account Info");
        if (ImGui::Button("Disconnect"))
        {
            onClickDisconnect();
        }

        ImGui::Text("eid : %d", entity_id);
        ImGui::Separator();
        ImGui::Text("render fps: %d / simulate fps: %d", render_frame, simulate_frame);
        ImGui::End();
    }
}
