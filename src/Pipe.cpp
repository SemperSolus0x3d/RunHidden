#include <stdexcept>
#include <string>
#include <array>
#include "Pipe.h"
#include "Utils.h"

Pipe::Pipe()
{
    using namespace std::string_literals;

    SECURITY_ATTRIBUTES securityAttributes;

    securityAttributes.bInheritHandle = TRUE;
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.lpSecurityDescriptor = NULL;

    BOOL retValue = CreatePipe(
        &m_ReadEnd,
        &m_WriteEnd,
        &securityAttributes,
        0
    );

    if (retValue == 0)
        throw std::runtime_error(
            "Failed to create pipe. Error code: "s + 
            std::to_string(GetLastError())
        );
}

Pipe::~Pipe()
{
    CloseReadEnd();
    CloseWriteEnd();
}

HANDLE Pipe::GetReadEnd()
{
    return m_ReadEnd;
}

HANDLE Pipe::GetWriteEnd()
{
    return m_WriteEnd;
}

void Pipe::CloseReadEnd()
{
    SafeCloseHandle(m_ReadEnd);
}

void Pipe::CloseWriteEnd()
{
    SafeCloseHandle(m_WriteEnd);
}

void Pipe::RedirectFrom(HANDLE sourceHandle)
{
    Redirect(sourceHandle, m_WriteEnd);
}

void Pipe::RedirectTo(HANDLE destinationHandle)
{
    Redirect(m_ReadEnd, destinationHandle);
}

void Pipe::ForbidReadEndInheritance()
{
    ForbidHandleInheritance(m_ReadEnd);
}

void Pipe::ForbidWriteEndInheritance()
{
    ForbidHandleInheritance(m_WriteEnd);
}

void Pipe::ForbidHandleInheritance(HANDLE handle)
{
    using namespace std::string_literals;

    if (SetHandleInformation(handle, HANDLE_FLAG_INHERIT, 0) == 0)
        throw std::runtime_error(
            "Failed to forbid handle inheritance. Error code: "s +
            std::to_string(GetLastError())
        );
}

void Pipe::Redirect(HANDLE source, HANDLE destination)
{
    DWORD bytesRead, bytesWritten;
    std::array<char, 4096> buffer;
    BOOL success = FALSE;

    while (true)
    {
        success = ReadFile(
            source,
            buffer.data(),
            buffer.size(),
            &bytesRead,
            NULL
        );

        if (!success || bytesRead == 0)
            break;

        success = WriteFile(
            destination,
            buffer.data(),
            bytesRead,
            &bytesWritten,
            NULL
        );

        if (!success)
            break;
    }
}