#include "InputStream.h"
#include "Utils.h"

InputStream::InputStream(HANDLE sourceHandle, bool isStdHandle) :
    m_SourceHandle(sourceHandle),
    m_IsSourceHandleStdHandle(isStdHandle)
{
    m_Pipe.ForbidWriteEndInheritance();
}

InputStream::InputStream(std::wstring filePath) :
    m_IsSourceHandleStdHandle(false)
{
    m_SourceHandle = OpenFile(filePath, GENERIC_READ, false);
    m_Pipe.ForbidWriteEndInheritance();
}

InputStream::~InputStream()
{
    CloseSourceHandle();
}

HANDLE InputStream::GetHandle()
{
    if (m_IsSourceHandleStdHandle)
        return m_SourceHandle;
    else
        return m_Pipe.GetReadEnd();
}

std::future<void> InputStream::CopyData()
{
    if (m_IsSourceHandleStdHandle)
        return std::async(std::launch::deferred, [](){});

    m_Pipe.CloseReadEnd();

    return std::async(std::launch::async, [this](){
        m_Pipe.RedirectFrom(m_SourceHandle);
        m_Pipe.CloseWriteEnd();
        CloseSourceHandle();
    });
}

void InputStream::CloseSourceHandle()
{
    if (!m_IsSourceHandleStdHandle)
        SafeCloseHandle(m_SourceHandle);
}
