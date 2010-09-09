#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Condition.h"
#include "Platform/Platform.h"
#include "Platform/Assert.h"

using namespace Helium;

Condition::Condition()
{
    m_Handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    if ( !m_Handle )
    {
        Helium::Print(TXT("Failed to create event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

Condition::~Condition()
{
    BOOL result = ::CloseHandle(m_Handle);
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to close event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

void Condition::Signal()
{
    BOOL result = ::SetEvent(m_Handle);
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to signal event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

void Condition::Reset()
{
    BOOL result = ::ResetEvent(m_Handle);
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to reset event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

bool Condition::Wait(u32 timeout)
{
    DWORD result = ::WaitForSingleObject(m_Handle, timeout);

    if ( timeout != 0xffffffff && result == WAIT_TIMEOUT )
    {
        return false;
    }

    if ( result != WAIT_OBJECT_0 )
    {
        Helium::Print(TXT("Failed to wait for event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }

    return true;
}
