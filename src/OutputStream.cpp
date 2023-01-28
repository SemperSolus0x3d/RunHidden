#include "OutputStream.h"
#include "Utils.h"

OutputStream::OutputStream(HANDLE destHandle, bool isStdHandle) :
    m_DestHandle(destHandle),
    m_IsDestHandleStdHandle(isStdHandle)
{
    m_Pipe.ForbidReadEndInheritance();
}

OutputStream::OutputStream(std::wstring filePath, bool append) :
    m_IsDestHandleStdHandle(false)
{
    m_DestHandle = OpenFile(filePath, GENERIC_WRITE, append);
    m_Pipe.ForbidReadEndInheritance();
}

OutputStream::~OutputStream()
{
    CloseDestHandle();
}

HANDLE OutputStream::GetHandle()
{
    if (m_IsDestHandleStdHandle)
        return m_DestHandle;
    else
        return m_Pipe.GetWriteEnd();
}

std::future<void> OutputStream::CopyData()
{
    if (m_IsDestHandleStdHandle)
        return std::async(std::launch::deferred, [](){});

    m_Pipe.CloseWriteEnd();

    return std::async(std::launch::async, [this](){
        m_Pipe.RedirectTo(m_DestHandle);
        m_Pipe.CloseReadEnd();
        CloseDestHandle();
    });
}

void OutputStream::CloseDestHandle()
{
    if (!m_IsDestHandleStdHandle)
        SafeCloseHandle(m_DestHandle);
}
