#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Condition.h"
#include "Platform/PlatformUtility.h"
#include "Platform/Assert.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] resetMode      Whether the condition should automatically reset after releasing a thread when signaled
///                           (RESET_MODE_AUTO) or whether the condition will remain signaled until Reset() is called
///                           (RESET_MODE_MANUAL).
/// @param[in] bInitialState  True to initialize this condition in the signaled state, false to initialize non-signaled.
Condition::Condition( EResetMode resetMode, bool bInitialState )
{
    HELIUM_ASSERT( static_cast< size_t >( resetMode ) < static_cast< size_t >( RESET_MODE_MAX ) );

    //m_Handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    m_Handle = ::CreateEvent( NULL, ( resetMode == RESET_MODE_MANUAL ), bInitialState, NULL );
    HELIUM_ASSERT( m_Handle );
    if ( !m_Handle )
    {
        Helium::Print(TXT("Failed to create event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

/// Destructor.
Condition::~Condition()
{
    BOOL result = ::CloseHandle(m_Handle);
    HELIUM_ASSERT( result );
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to close event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

/// Signal this condition.
///
/// This is used to notify threads waiting on this signal to wake up.  The effect differs based on whether this is a
/// manual-reset or auto-reset condition.
/// - For manual-reset conditions, all waiting threads will be signaled, and the condition will be left in the signaled
///   state until Reset() is called.
/// - For auto-reset conditions, the condition will remain signaled until one waiting thread is released, after which
///   the condition will automatically be reset.  If no threads are waiting when the condition is signaled, it will
///   remain in the signaled state until either a thread does attempt to wait on it or Reset() is called.
///
/// @see Wait(), Reset()
void Condition::Signal()
{
    BOOL result = ::SetEvent(m_Handle);
    HELIUM_ASSERT( result );
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to signal event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

/// Reset this condition to the non-signaled state.
///
/// @see Signal(), Wait()
void Condition::Reset()
{
    BOOL result = ::ResetEvent(m_Handle);
    HELIUM_ASSERT( result );
    if ( result != TRUE )
    {
        Helium::Print(TXT("Failed to reset event (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
}

/// Wait indefinitely on this condition.
///
/// This causes the current thread to sleep until the condition is signaled.  If the condition is currently in the
/// signaled state, the thread will wake up immediately.  The behavior of the condition when signaled is determined by
/// the reset mode of the condition (see Signal() for more information).
///
/// @return  True if the condition was signaled, false if waiting was interrupted.
///
/// @see Signal, Reset()
bool Condition::Wait()
{
    DWORD result = ::WaitForSingleObject( m_Handle, INFINITE );

    return ( result == WAIT_OBJECT_0 );
}

/// Wait on this condition for up to a given period of time.
///
/// This causes the current thread to sleep until the condition is signaled.  If the condition is currently in the
/// signaled state, the thread will wake up immediately.  The behavior of the condition when signaled is determined by
/// the reset mode of the condition (see Signal() for more information).
///
/// @param[in] timeoutMs  Maximum number of milliseconds to wait for the condition to be signaled.
///
/// @return  True if the condition was signaled, false if waiting was interrupted.
///
/// @see Signal, Reset()
bool Condition::Wait( uint32_t timeout )
{
    DWORD result = ::WaitForSingleObject(m_Handle, timeout);

    if ( result != WAIT_OBJECT_0 )
    {
        if ( result != WAIT_TIMEOUT )
        {
            Helium::Print(TXT("Failed to wait for event (%s)\n"), Helium::GetErrorString().c_str());
            HELIUM_BREAK();
        }

        return false;
    }

    return true;
}
