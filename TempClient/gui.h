#pragma once

#include "pre.h"

#include "widget/login_window.h"
#include "widget/scene_info.h"
#include "widget/log_widget.h"
#include "widget/console_widget.h"

#define LOG_INFO(Log, ...) Gui::instance().log.AddLog(std::format(\
"[info] {} \n", std::format(Log, ##__VA_ARGS__)).c_str());
#define LOG_WARN(Log, ...) Gui::instance().log.AddLog(std::format(\
"[warn] {} \n", std::format(Log, ##__VA_ARGS__)).c_str());
#define LOG_ERR(Log, ...) {Gui::instance().log.AddLog(std::format(\
"[error] {} ", std::format(Log, ##__VA_ARGS__)).c_str());\
 Gui::instance().log.AddLog(std::format("{} line:[{}]\n", \
std::source_location::current().file_name(),\
std::source_location::current().line()).c_str());}

#define CHAT_LOG(Log, ...) Gui::instance().console.AddLog(std::format(\
" {} \n", ##__VA_ARGS__).c_str());

#ifdef _DEBUG
#define DEBUG_RETURN(boolean, Log, ...) if(!boolean){\
LOG_ERR(Log,##__VA_ARGS__); return;}

#define DEBUG_RETURN_VALUE(boolean, value, Log, ...) if(!boolean){\
LOG_ERR(Log,##__VA_ARGS__); return value;}
#else
#define DEBUG_RETURN(boolean, Log, ...);
#define DEBUG_RETURN_VALUE(boolean, value, Log, ...);
#endif

class Gui : public Singleton<Gui>
{
public:
    void Initialize();

    static void GuiList()
    {
        ImGui::ShowDemoWindow();
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