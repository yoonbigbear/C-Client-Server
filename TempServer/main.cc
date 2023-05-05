#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif
#include <stdlib.h>
#include <crtdbg.h>

#include "pre.h"
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

#include "server.h"
#include "db/db.h"

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

BOOL CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL+C signal.
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT: // CTRL+CLOSE: confirm! that the user wants to exit.
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    default:
        return FALSE;
    }
}

int main()
{
    //DB::instance().run_test(NANODBC_TEXT("Driver={MySQL ODBC 8.0 Unicode Driver};Server=localhost;Port=3306;Database=account;Uid=root;Pwd=admin"));
    //DB::instance().run_test(NANODBC_TEXT("Driver={MySQL ODBC 8.0 ANSI Driver};Server=localhost;Port=3306;Database=game;Uid=root;Pwd=admin"));

    BOOL fSuccess = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

    SetUnhandledExceptionFilter(ExceptionCallBack);
    //_CrtSetBreakAlloc(179);
    LOG_INFO("Create Server!!");
    Server server(11000);

    LOG_INFO("Server Initialize!!");
    if (!server.Initialize())
        return -1;

    SimulateTimer<20.0> timer;
    timer.Reset();
    LOG_INFO("Server Start!!");
    while (fSuccess)
    {
        timer.Frame();
        while (timer.time_acc > timer.FRAMERATE)
        {
            timer.time_acc -= timer.FRAMERATE;
            server.Update(static_cast<float>(timer.FRAMERATE));
        }
    }
    spdlog::shutdown();
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtDumpMemoryLeaks();
    return 0;
}