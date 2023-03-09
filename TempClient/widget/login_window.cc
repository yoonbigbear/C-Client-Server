#include "login_window.h"

#include "imgui.h"

void LoginWindow::Draw()
{
    if (!login)
    {
        ImGui::Begin("Login");

        const char* ip = "127.0.0.1";
        const char* ppw = "";
        uint16_t port = 11000;

        if (ImGui::Button("ConnectServer"))
        {
            onClickConnect(ip, port);
        }

        ImGui::End();
    }
    else
    {
        ImGui::Begin("Account Info");

        ImGui::Text("eid : %d", entity_id);

        ImGui::End();
    }
}
