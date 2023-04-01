#include "gui.h"




void Gui::Initialize()
{
    
}

void Gui::Draw()
{
    login.Draw();
    level_info.Draw();
    log.Draw("Log");
    console.Draw("Conosle", NULL);
}
