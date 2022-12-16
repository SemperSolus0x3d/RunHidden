#include <iostream>
#include <string>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

std::wstring GetCommand(wchar_t** args, int beginIndex, int length);

int wmain(int argc, wchar_t** argv)
{
    if (argc == 1)
    {
        std::wcout << L"No command given" << std::endl;
        std::cin.get();
        return -1;
    }

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    auto command = GetCommand(argv, 1, argc);

    return _wsystem(command.c_str());
}

bool ContainsArgumentSeparator(wchar_t* str)
{
    return
        wcschr(str, L' ') != NULL ||
        wcschr(str, L'\t') != NULL;
}

std::wstring GetCommand(wchar_t** args, int begin, int length)
{
    std::wstringstream stream;

    for (int i = begin; i < length; i++)
        if (ContainsArgumentSeparator(args[i]))
            stream << L'\"' << args[i] << L"\" ";
        else
            stream << args[i] << L' ';

    return stream.str();
}
