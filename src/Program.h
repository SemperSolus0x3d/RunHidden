#pragma once
#include <memory>
#include <vector>
#include <future>
#include <Windows.h>
#include "CommandLineArgs.h"
#include "InputStream.h"
#include "OutputStream.h"

class Program
{
public:
    Program(int argc, wchar_t** argv);
    ~Program();
    int Run();
    void Stop();
private:
    CommandLineArgs m_Args;
    STARTUPINFOW m_StartupInfo;
    PROCESS_INFORMATION m_ProcessInfo;
    HANDLE m_Job;
    HANDLE m_CompletionPort;
    HWND m_ConsoleWindowHandle;

    std::shared_ptr<Stream> m_StdoutStream;
    std::shared_ptr<Stream> m_StderrStream;
    std::shared_ptr<Stream> m_StdinStream;

    void ParseCommandLineArgs(int argc, wchar_t** argv);
    void PrintHelp();
    void InitializeStartupInfo();
    void InitializeProcessInfo();
    void CreateJobAndCompletionPort();
    void CreateChildProcess();
    void WaitUntilClildProcessCompletes();
    std::vector<std::future<void>> StartIoRedirection();
    void HideConsoleWindow();
    void ShowConsoleWindow();
};