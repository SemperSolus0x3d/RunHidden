#include <iostream>
#include <string>

#include "Program.h"

int wmain(int argc, wchar_t** argv)
{
    try
    {
        Program program(argc, argv);
        program.Run();
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
