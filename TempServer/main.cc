#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "pre.h"
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#include "server.h"
#include "timer.h"

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


int main()
{
    SetUnhandledExceptionFilter(ExceptionCallBack);

    LOG_INFO("Create Server!!");
    Server server(11000);

    LOG_INFO("Server Initialize!!");
    server.Initialize();

    SimulateTimer<20.0> timer;
    timer.Reset();
    LOG_INFO("Server Start!!");
    while (true)
    {
        timer.Frame();
        while (timer.time_acc > timer.FRAMERATE)
        {
            timer.time_acc -= timer.FRAMERATE;
            server.Update(static_cast<float>(timer.FRAMERATE));
        }
    }
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return 0;
}