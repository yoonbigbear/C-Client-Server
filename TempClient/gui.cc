#include "gui.h"

void Gui::Draw()
{
    login.Draw();
    level_info.Draw();
    log.Draw("Log");
    console.Draw("Conosle", NULL);
}
