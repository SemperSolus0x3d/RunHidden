#pragma once
#include <string>

class CommandLineArgs
{
public:
    std::wstring StdinFile;
    std::wstring StdoutFile;
    std::wstring StderrFile;
    std::wstring CommandLine;
    bool AppendStdoutFile = false;
    bool AppendStderrFile = false;
};
