#pragma once
#include "Stream.h"
#include "Pipe.h"

class InputStream : public Stream
{
public:
    InputStream(HANDLE sourceHandle, bool isStdHandle = true);
    InputStream(std::wstring filePath);

    ~InputStream();

    HANDLE GetHandle();
    std::future<void> CopyData();
private:
    HANDLE m_SourceHandle;
    bool m_IsSourceHandleStdHandle;
    Pipe m_Pipe;

    void CloseSourceHandle();
};
