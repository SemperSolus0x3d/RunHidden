#pragma once
#include "Windows.h"

class Pipe
{
public:
    Pipe();
    ~Pipe();

    HANDLE GetReadEnd();
    HANDLE GetWriteEnd();

    void CloseReadEnd();
    void CloseWriteEnd();

    void ForbidReadEndInheritance();
    void ForbidWriteEndInheritance();

    void RedirectTo(HANDLE destinationHandle);
    void RedirectFrom(HANDLE sourceHandle);
private:
    HANDLE m_ReadEnd;
    HANDLE m_WriteEnd;

    void ForbidHandleInheritance(HANDLE handle);
    void Redirect(HANDLE from, HANDLE to);
};
