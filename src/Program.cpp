#include <iostream>
#include <exception>
#include <stdexcept>
#include <string_view>
#include "Program.h"
#include "Utils.h"

const ULONG_PTR CompletionKey = 52534;

Program::Program(int argc, wchar_t** argv)
{
    ParseCommandLineArgs(argc, argv);
    InitializeStartupInfo();
    InitializeProcessInfo();
    CreateJobAndCompletionPort();
}

Program::~Program()
{
    SafeCloseHandle(m_ProcessInfo.hProcess);
    SafeCloseHandle(m_ProcessInfo.hThread);
    SafeCloseHandle(m_Job);
    SafeCloseHandle(m_CompletionPort);
}

void Program::Run()
{
    using namespace std::string_literals;

    if (!m_Args.DoNotHideConsole)
        ShowWindow(GetConsoleWindow(), SW_HIDE);

    CreateChildProcess();

    AssignProcessToJobObject(m_Job, m_ProcessInfo.hProcess);

    auto futures = StartIoRedirection();

    ResumeThread(m_ProcessInfo.hThread);
    WaitUntilClildProcessCompletes();

    for (auto& future : futures)
        future.wait();
}

void Program::Stop()
{
    TerminateJobObject(m_Job, 0);
}

void Program::ParseCommandLineArgs(int argc, wchar_t** argv)
{
    using namespace std::string_literals;

    for (int i = 1; i < argc; i++)
    {
        std::wstring_view arg = argv[i];
        std::wstring_view nextArg;

        if (i + 1 < argc)
            nextArg = argv[i + 1];

        if (arg == L"-i")
        {
            m_Args.StdinFile = nextArg;
            i++;
        }
        else if (arg == L"-o")
        {
            m_Args.StdoutFile = nextArg;
            i++;
        }
        else if (arg == L"-o+")
        {
            m_Args.StdoutFile = nextArg;
            m_Args.AppendStdoutFile = true;
            i++;
        }
        else if (arg == L"-e")
        {
            m_Args.StderrFile = nextArg;
            i++;
        }
        else if (arg == L"-e+")
        {
            m_Args.StderrFile = nextArg;
            m_Args.AppendStderrFile = true;
            i++;
        }
        else if (arg == L"-h" || arg == L"--help")
        {
            PrintHelp();
        }
        else if (arg == L"--no-hide")
        {
            m_Args.DoNotHideConsole = true;
        }
        else if (arg == L"--")
        {
            for (int j = i + 1; j < argc; j++)
                m_Args.CommandLine += L"\""s + argv[j] + L"\" ";

            break;
        }
    }

    if (m_Args.CommandLine.empty())
        throw std::runtime_error("No command specified");
}

void Program::PrintHelp()
{
    std::wcout
        << std::endl
        << L" RunHidden | Copyright (c) 2022-2023 SemperSolus0x3d" << std::endl
        << std::endl
        << L" Usage: RunHidden [-o[+] <FILE|->] "
            L"[-e[+] <FILE|->] [-i <FILE->] -- COMMAND [ARGS...]" << std::endl
        << std::endl
        << L" Run specified COMMAND in console window which immediately hides itself." << std::endl
        << std::endl
        << L" -o[+] FILE    Redirect stdout to FILE. Special value \'-\' can be used" << std::endl
        << L"               to redirect child process stdout to RunHidden stdout." << std::endl
        << L"               The \'-o+\' version of this key appends data to file" << std::endl
        << L"               instead of rewriting it." << std::endl
        << std::endl
        << L" -e[+] FILE    Redirect stderr to FILE. Special value \'-\' can be used" << std::endl
        << L"               to redirect child process stderr to RunHidden stderr." << std::endl
        << L"               The \'-e+\' version of this key appends data to file" << std::endl
        << L"               instead of rewriting it." << std::endl
        << std::endl
        << L" -i FILE       Read data from FILE and pass it to child process stdin." << std::endl
        << L"               Special value '-' can be used to pass RunHidden stdin to" << std::endl
        << L"               child process stdin" << std::endl
        << L" --no-hide     Do not hide the console window (useful for debugging)" << std::endl
        << std::endl
        << L" --            Do not treat any more arguments as options." << std::endl
        << std::endl
        << L" -h, --help    Show this help message" << std::endl
        << std::endl;
}

void Program::InitializeStartupInfo()
{
    ZeroMemory(&m_StartupInfo, sizeof(m_StartupInfo));
    m_StartupInfo.cb = sizeof(m_StartupInfo);

    m_StartupInfo.hStdOutput =
        m_StartupInfo.hStdError =
        m_StartupInfo.hStdInput = INVALID_HANDLE_VALUE;

    m_StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    m_StartupInfo.wShowWindow = SW_HIDE;

    if (!m_Args.StdoutFile.empty())
    {
        if (m_Args.StdoutFile == L"-")
            m_StdoutStream = std::make_shared<OutputStream>(
                GetStdHandle(STD_OUTPUT_HANDLE)
            );
        else
            m_StdoutStream = std::make_shared<OutputStream>(
                m_Args.StdoutFile,
                m_Args.AppendStdoutFile
            );

        m_StartupInfo.hStdOutput = m_StdoutStream->GetHandle();
    }

    if (!m_Args.StderrFile.empty())
    {
        if (m_Args.StderrFile == L"-")
            m_StderrStream = std::make_shared<OutputStream>(
                GetStdHandle(STD_ERROR_HANDLE)
            );
        else
            m_StderrStream = std::make_shared<OutputStream>(
                m_Args.StderrFile,
                m_Args.AppendStderrFile
            );

        m_StartupInfo.hStdError = m_StderrStream->GetHandle();
    }

    if (!m_Args.StdinFile.empty())
    {
        if (m_Args.StdinFile == L"-")
            m_StdinStream = std::make_shared<InputStream>(
                GetStdHandle(STD_INPUT_HANDLE)
            );
        else
            m_StdinStream = std::make_shared<InputStream>(
                m_Args.StdinFile
            );

        m_StartupInfo.hStdInput = m_StdinStream->GetHandle();
    }
}

void Program::InitializeProcessInfo()
{
    ZeroMemory(&m_ProcessInfo, sizeof(m_ProcessInfo));
}

void Program::CreateJobAndCompletionPort()
{
    using namespace std::string_literals;

    m_Job = CreateJobObjectW(NULL, NULL);

    if (m_Job == INVALID_HANDLE_VALUE)
        throw std::runtime_error(
            "Job creation failed. Error code: "s +
            std::to_string(GetLastError())
        );

    m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    if (m_CompletionPort == NULL)
        throw std::runtime_error(
            "Completion port creation failed. Error code: "s +
            std::to_string(GetLastError())
        );

    JOBOBJECT_ASSOCIATE_COMPLETION_PORT info;
    info.CompletionKey = (PVOID)CompletionKey;
    info.CompletionPort = m_CompletionPort;

    if (!SetInformationJobObject(
        m_Job,
        JobObjectAssociateCompletionPortInformation,
        &info,
        sizeof(info))
    )
        throw std::runtime_error(
            "Failed to associate job with completion port. Error code: "s +
            std::to_string(GetLastError())
        );
}

void Program::WaitUntilClildProcessCompletes()
{
    while(true)
    {
        DWORD completionCode;
        ULONG_PTR completionKey;
        LPOVERLAPPED overlapped;

        if (!GetQueuedCompletionStatus(
            m_CompletionPort,
            &completionCode,
            &completionKey,
            &overlapped,
            INFINITE
        ))
            break;

        if (completionKey == CompletionKey && 
            completionCode == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO)
            break;
    }
}

void Program::CreateChildProcess()
{
    using namespace std::string_literals;

    if (!CreateProcessW(
        NULL,
        &m_Args.CommandLine[0],
        NULL,
        NULL,
        TRUE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &m_StartupInfo,
        &m_ProcessInfo
    ))
        throw std::runtime_error(
            "Process creation failed. Error code: "s +
            std::to_string(GetLastError())
        );
}

std::vector<std::future<void>> Program::StartIoRedirection()
{
    std::vector<std::future<void>> futures;

    auto streams = std::vector({
        m_StdinStream,
        m_StdoutStream,
        m_StderrStream
    });

    for (auto& stream : streams)
        if (stream != nullptr)
            futures.push_back(stream->CopyData());

    return futures;
}
