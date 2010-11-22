#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Semaphore.h"
#include "Platform/PlatformUtility.h"
#include "Platform/Assert.h"

using namespace Helium;

Semaphore::Semaphore()
{
    m_Handle = ::CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
    if ( m_Handle == NULL )
    {
        Helium::Print(TXT("Failed to create semaphore (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

Semaphore::~Semaphore()
{
    BOOL result = ::CloseHandle(m_Handle);
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to close semaphore (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

void Semaphore::Increment()
{
    LONG count = 0;
    BOOL result = ::ReleaseSemaphore(m_Handle, 1, &count);
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to inrement semaphore from %d (%s)\n"), count, Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

void Semaphore::Decrement()
{
    DWORD result = ::WaitForSingleObject(m_Handle, INFINITE);
    if ( result != WAIT_OBJECT_0 )
    {
        Helium::Print(TXT("Failed to decrement semaphore (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

void Semaphore::Reset()
{
    BOOL result = ::CloseHandle(m_Handle);
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to close semaphore (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }

    m_Handle = ::CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
    if ( m_Handle == NULL )
    {
        Helium::Print(TXT("Failed to create semaphore (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}
