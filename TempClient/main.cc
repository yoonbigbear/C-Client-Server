#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "pre.h"
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#include "app.h"

//dump
LONG __stdcall ExceptionCallBack(EXCEPTION_POINTERS* e)
{
    MINIDUMP_EXCEPTION_INFORMATION info = { 0 };
    info.ThreadId = ::GetCurrentThreadId();
    info.ExceptionPointers = e;
    info.ClientPointers = false;

    const wchar_t* stemp = L"dump.dmp";

    HANDLE file_handle = CreateFile(stemp, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, MiniDumpNormal, &info,
        NULL, NULL);

    return 0;
}


int main(int, char**)
{
    SetUnhandledExceptionFilter(ExceptionCallBack);


    ShowWindow(GetConsoleWindow(), SW_HIDE);

    auto app = std::make_unique<App>();
    if (!app->Initialize())
        return 0;

    app->Run();
    //app->RunSingleThread();

    app.release();
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return 0;
}
