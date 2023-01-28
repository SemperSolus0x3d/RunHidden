#pragma once
#include <future>
#include <string>
#include <Windows.h>

class Stream
{
public:
    virtual ~Stream() = 0;

    virtual HANDLE GetHandle() = 0;
    virtual std::future<void> CopyData() = 0;
protected:
    HANDLE OpenFile(const std::wstring& path, DWORD accessFlags, bool append);
};
