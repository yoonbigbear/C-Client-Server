#include "pre.h"
#include "server.h"

#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

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

    LOG_INFO("Create Server..");
    Server server(11000);

    LOG_INFO("Server Initialize..");
    server.Initialize();

    LOG_INFO("Run MainLoop");
    while (true)
    {
        server.Update();
    }
    return 0;
}