#pragma once
#include "Stream.h"
#include "Pipe.h"

class OutputStream : public Stream
{
public:
    OutputStream(HANDLE destHandle, bool isStdHandle = true);
    OutputStream(std::wstring filePath, bool append);

    ~OutputStream();

    HANDLE GetHandle();
    std::future<void> CopyData();
private:
    HANDLE m_DestHandle;
    bool m_IsDestHandleStdHandle;
    Pipe m_Pipe;

    void CloseDestHandle();
};
