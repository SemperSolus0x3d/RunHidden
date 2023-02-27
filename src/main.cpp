#include <iostream>
#include <string>
#include <memory>

#include "Program.h"

BOOL WINAPI HandleCtrlEvent(_In_ DWORD ctrlType);

std::shared_ptr<Program> ProgramPtr;

int wmain(int argc, wchar_t** argv)
{
    SetConsoleCtrlHandler(&HandleCtrlEvent, TRUE);

    try
    {
        ProgramPtr = std::make_shared<Program>(argc, argv);
        ProgramPtr->Run();
        return 0;
    }
    catch (const std::exception& ex)
    {
        ShowWindow(GetConsoleWindow(), SW_SHOW);
        std::wcout << L"Error: " << ex.what() << std::endl;
        std::wcin.get();
        return -1;
    }
}

BOOL WINAPI HandleCtrlEvent(_In_ DWORD ctrlType)
{
    switch(ctrlType)
    {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        {
            ProgramPtr->Stop();

            return TRUE;
        }
        default:
            return FALSE;
    }
}
