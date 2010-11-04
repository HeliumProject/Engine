//----------------------------------------------------------------------------------------------------------------------
// Log.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/Log.h"

namespace Lunar
{
    /// Constructor.
    Log::Log()
        : m_level( LOG_INFO )
        , m_lastMessageLevel( LOG_INVALID )
        , m_bNewLine( true )
    {
    }

    /// Destructor.
    Log::~Log()
    {
    }

    /// Set the current logging level.
    ///
    /// @param[in] level  Logging level.
    ///
    /// @see GetLevel()
    void Log::SetLevel( ELogLevel level )
    {
        ScopeLock< LwMutex > scopeLock( m_mutex );

        HELIUM_ASSERT( static_cast< size_t >( level ) < static_cast< size_t >( LOG_MAX ) );
        m_level = level;
    }

    /// Write out a formatted message to this log.
    ///
    /// @param[in] level    Logging level.
    /// @param[in] pFormat  Format string.
    /// @param[in] ...      Format arguments.
    void Log::Output( ELogLevel level, const tchar_t* pFormat, ... )
    {
        va_list argList;
        va_start( argList, pFormat );
        OutputVa( level, pFormat, argList );
        va_end( argList );
    }

    /// Write out a message to this log.
    ///
    /// @param[in] level     Logging level.
    /// @param[in] rMessage  Message text.
    void Log::Output( ELogLevel level, const String& rMessage )
    {
        if( level < m_level )
        {
            return;
        }

        ScopeLock< LwMutex > scopeLock( m_mutex );

        if( m_bNewLine || level != m_lastMessageLevel )
        {
            OutputImpl( GetLevelString( level ) );
        }

        m_bNewLine = false;
        m_lastMessageLevel = level;

        const tchar_t* pMessage = rMessage.GetData();
        if( pMessage )
        {
            size_t messageLength = StringLength( pMessage );
            if( messageLength != 0 )
            {
                OutputImpl( pMessage );
                m_bNewLine = ( pMessage[ messageLength - 1 ] == TXT( '\n' ) );
            }
        }
    }

    /// Write out a formatted message to this log using a variable argument list.
    ///
    /// @param[in] level    Logging level.
    /// @param[in] pFormat  Format string.
    /// @param[in] argList  Initialized variable argument list for the format arguments (va_start() should have already
    ///                     been called on this as necessary).
    void Log::OutputVa( ELogLevel level, const tchar_t* pFormat, va_list argList )
    {
        HELIUM_ASSERT( pFormat );

        ScopeLock< LwMutex > scopeLock( m_mutex );

        if( level < m_level )
        {
            return;
        }

        if( m_bNewLine || level != m_lastMessageLevel )
        {
            OutputImpl( GetLevelString( level ) );
        }

        m_bNewLine = false;
        m_lastMessageLevel = level;

        tchar_t buffer[ DEFAULT_MESSAGE_BUFFER_SIZE ];

        va_list argListTemp = argList;
        int result = StringFormatVa( buffer, L_ARRAY_COUNT( buffer ), pFormat, argListTemp );

        if( static_cast< unsigned int >( result ) < L_ARRAY_COUNT( buffer ) )
        {
            OutputImpl( buffer );
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
            OutputImpl( pBuffer );
            m_bNewLine = ( pBuffer[ result - 1 ] == TXT( '\n' ) );

            allocator.Free( pBuffer );
        }
    }

    /// Get the string to display to indicate the specified logging level.
    ///
    /// @param[in] level  Logging level.
    ///
    /// @return  Logging level string.
    const tchar_t* Log::GetLevelString( ELogLevel level )
    {
        HELIUM_ASSERT( static_cast< size_t >( level ) < static_cast< size_t >( LOG_MAX ) );

        switch( level )
        {
            case LOG_DEBUG:
            {
                return TXT( "[D] " );
            }

            case LOG_INFO:
            {
                return TXT( "[I] " );
            }

            case LOG_WARNING:
            {
                return TXT( "[W] " );
            }

            case LOG_ERROR:
            {
                return TXT( "[E] " );
            }
        }

        return TXT( "[?] " );
    }
}
