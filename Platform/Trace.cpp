//#include "CorePch.h"
#include "Platform/Trace.h"

#include "Platform/Debug.h"

#include <cstdarg>

using namespace Helium;

Helium::Trace Helium::g_Trace;

/// Constructor.
Helium::Trace::Trace()
    : m_level( TRACE_INFO )
    , m_lastMessageLevel( TRACE_INVALID )
    , m_bNewLine( true )
{
}

/// Destructor.
Helium::Trace::~Trace()
{
}

/// Set the current logging level.
///
/// @param[in] level  Logging level.
///
/// @see GetLevel()
void Helium::Trace::SetLevel( ETraceLevel level )
{
    MutexScopeLock scopeLock( m_mutex );

    HELIUM_ASSERT( static_cast< size_t >( level ) < static_cast< size_t >( TRACE_MAX ) );
    m_level = level;
}

/// Write out a formatted message to this log.
///
/// @param[in] level    Logging level.
/// @param[in] pFormat  Format string.
/// @param[in] ...      Format arguments.
void Helium::Trace::Output( ETraceLevel level, const tchar_t* pFormat, ... )
{
    va_list argList;
    va_start( argList, pFormat );
    OutputVa( level, pFormat, argList );
    va_end( argList );
}

#pragma TODO( "LUNAR MERGE - Enable the following once String has been merged over." )
#if 0
/// Write out a message to this log.
///
/// @param[in] level     Logging level.
/// @param[in] rMessage  Message text.
void Helium::Trace::Output( ETraceLevel level, const String& rMessage )
{
    if( level < m_level )
    {
        return;
    }

    MutexScopeLock scopeLock( m_mutex );

    if( m_bNewLine || level != m_lastMessageLevel )
    {
        OutputImplementation( GetLevelString( level ) );
    }

    m_bNewLine = false;
    m_lastMessageLevel = level;

    const tchar_t* pMessage = rMessage.GetData();
    if( pMessage )
    {
        size_t messageLength = StringLength( pMessage );
        if( messageLength != 0 )
        {
            OutputImplementation( pMessage );
            m_bNewLine = ( pMessage[ messageLength - 1 ] == TXT( '\n' ) );
        }
    }
}
#endif

/// Write out a formatted message to this log using a variable argument list.
///
/// @param[in] level    Logging level.
/// @param[in] pFormat  Format string.
/// @param[in] argList  Initialized variable argument list for the format arguments (va_start() should have already
///                     been called on this as necessary).
void Helium::Trace::OutputVa( ETraceLevel level, const tchar_t* pFormat, va_list argList )
{
    HELIUM_ASSERT( pFormat );

#pragma TODO( "LUNAR MERGE - Re-enable once memory routines have been merged over." )
#if 1
    ( void )level;
    ( void )pFormat;
    ( void )argList;
#else
    MutexScopeLock scopeLock( m_mutex );

    if( level < m_level )
    {
        return;
    }

    if( m_bNewLine || level != m_lastMessageLevel )
    {
        OutputImplementation( GetLevelString( level ) );
    }

    m_bNewLine = false;
    m_lastMessageLevel = level;

    tchar_t buffer[ DEFAULT_MESSAGE_BUFFER_SIZE ];

    va_list argListTemp = argList;
    int result = StringFormatVa( buffer, HELIUM_ARRAY_COUNT( buffer ), pFormat, argListTemp );

    if( static_cast< unsigned int >( result ) < HELIUM_ARRAY_COUNT( buffer ) )
    {
        OutputImplementation( buffer );
        m_bNewLine = ( buffer[ result - 1 ] == TXT( '\n' ) );

        return;
    }

    if( result < 0 )
    {
        argListTemp = argList;
        result = StringFormatVa( NULL, 0, pFormat, argListTemp );
        HELIUM_ASSERT( result >= 0 );
    }

    size_t bufferSize = static_cast< size_t >( result ) + 1;

    DefaultAllocator allocator;
    tchar_t* pBuffer = static_cast< tchar_t* >( allocator.Allocate( sizeof( tchar_t ) * bufferSize ) );
    HELIUM_ASSERT( pBuffer );
    if( pBuffer )
    {
        argListTemp = argList;
        result = StringFormatVa( pBuffer, bufferSize, pFormat, argListTemp );

        HELIUM_ASSERT( result == static_cast< int >( bufferSize - 1 ) );
        OutputImplementation( pBuffer );
        m_bNewLine = ( pBuffer[ result - 1 ] == TXT( '\n' ) );

        allocator.Free( pBuffer );
    }
#endif
}

/// Write out a message to this log.
///
/// @param[in] pMessage  Message text.
void Helium::Trace::OutputImplementation( const tchar_t* pMessage )
{
    HELIUM_ASSERT( pMessage );

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    DebugLog( pMessage );
#endif
}

/// Get the string to display to indicate the specified logging level.
///
/// @param[in] level  Logging level.
///
/// @return  Logging level string.
const tchar_t* Helium::Trace::GetLevelString( ETraceLevel level )
{
    HELIUM_ASSERT( static_cast< size_t >( level ) < static_cast< size_t >( TRACE_MAX ) );

    switch( level )
    {
        case TRACE_DEBUG:
        {
            return TXT( "[D] " );
        }

        case TRACE_INFO:
        {
            return TXT( "[I] " );
        }

        case TRACE_WARNING:
        {
            return TXT( "[W] " );
        }

        case TRACE_ERROR:
        {
            return TXT( "[E] " );
        }
    }

    return TXT( "[?] " );
}
