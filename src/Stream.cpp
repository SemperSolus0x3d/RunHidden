#include "Stream.h"

Stream::~Stream() {}

HANDLE Stream::OpenFile(const std::wstring& path, DWORD accessFlags, bool append)
{
    using namespace std::string_literals;

    HANDLE fileHandle = CreateFileW(
        path.c_str(),
        accessFlags,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE)
        throw std::runtime_error(
            "Could not open file. Error code: "s +
            std::to_string(GetLastError())
        );

    if (append)
        SetFilePointer(fileHandle, 0, NULL, FILE_END);

    return fileHandle;
}
