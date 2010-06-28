#pragma once

#include "Platform/Compiler.h"
#include "Platform/Error.h"

#include <stdio.h>
#include <stdarg.h>

#include <string>
#include <exception>

namespace Nocturnal
{
    //
    // Constants
    //

    const size_t ERROR_STRING_BUF_SIZE = 768; 

    //
    // Basic Insomniac Exception
    //  Try to only throw in "error" cases. Examples:
    //   * A file format API trying to open a file that doesn't exist (the client api should check if it exists if it was graceful execution)
    //   * A disc drive or resource is out of space and cannot be written to
    //   * A network port is taken and cannot be bound
    //

    class Exception : public std::exception
    {
    protected:
        mutable tstring m_Message;

    protected:
        Exception()
        {

        }

    public:
        Exception( const tchar *msgFormat, ... )
        {
            va_list msgArgs;
            va_start( msgArgs, msgFormat );
            SetMessage( msgFormat, msgArgs );
            va_end( msgArgs );
        }

        //
        // These accessors are thow that re-throw blocks can amend the exception message
        //

        tstring& Get()
        {
            return m_Message;
        }

        const tstring& Get() const
        {
            return m_Message;
        }

        void Set(const tstring& message)
        {
            m_Message = message;
        }

        //
        // This allow operation with std::exception case statements
        //

        virtual const tchar* What() const
        {
            return m_Message.c_str();
        }

    protected:
        void SetMessage( const tchar* msgFormat, ... )
        {
            va_list msgArgs;
            va_start( msgArgs, msgFormat );
            SetMessage( msgFormat, msgArgs );
            va_end( msgArgs );
        }

        void SetMessage( const tchar* msgFormat, va_list msgArgs )
        {
            tchar msgBuffer[ERROR_STRING_BUF_SIZE];

            _vsntprintf( msgBuffer, sizeof(msgBuffer), msgFormat, msgArgs );
            msgBuffer[sizeof(msgBuffer) - 1] = 0; 

            m_Message = msgBuffer;
        }
    };


    //
    // PlatformException class
    //

    class PlatformException : public Nocturnal::Exception
    {
    protected:
        u32 m_ErrorCode;

    protected:
        PlatformException() throw() // hide default c_tor
            : m_ErrorCode( 0 )
        {

        }

    public:
        PlatformException( const tchar *msgFormat, ... )
            : m_ErrorCode( Platform::GetLastError() )
        {
            va_list msgArgs;
            va_start( msgArgs, msgFormat );
            SetMessage( msgFormat, msgArgs );
            va_end( msgArgs );
        }

        PlatformException( u32 error, const tchar *msgFormat, ... )
            : m_ErrorCode( error )
        {
            va_list msgArgs;
            va_start( msgArgs, msgFormat );
            SetMessage( msgFormat, msgArgs );
            va_end( msgArgs );
        }

        virtual int GetErrorCode() const throw()
        {
            return m_ErrorCode;
        }

    protected:
        virtual void SetMessage( const tchar *msgFormat, va_list msgArgs )
        {
            __super::SetMessage( msgFormat, msgArgs );

            // append system error information
            m_Message += TXT( " (" ) + Platform::GetErrorString(m_ErrorCode) + TXT( ")" );
        }
    };
}

#define DEFINE_EXCEPTION_CLASS \
class Exception : public Nocturnal::Exception \
{ \
protected: \
    Exception() \
{ \
    \
} \
public: \
    Exception( const tchar *msgFormat, ... ) \
{ \
    va_list msgArgs; \
    va_start( msgArgs, msgFormat ); \
    SetMessage( msgFormat, msgArgs ); \
    va_end( msgArgs ); \
} \
};

#define DEFINE_INIT_EXCEPTIONS_CLASSES \
class NotInitializedException : public Exception \
{ \
public: \
    NotInitializedException() : Exception( "Cannot use API, it has not been initialized!" ) {} \
}; \
class AlreadyInitializedException : public Exception \
{ \
public: \
    AlreadyInitializedException() : Exception( "API has already been initialized!" ) {} \
};
