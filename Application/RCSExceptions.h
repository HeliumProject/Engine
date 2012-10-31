#pragma once

#include "Platform/Exception.h"

namespace Helium
{
    namespace RCS
    {
        class Exception : public Helium::Exception
        {
        public:
            Exception( const tchar_t *msgFormat, ... )
            {
                va_list msgArgs;
                va_start( msgArgs, msgFormat );
                SetMessage( msgFormat, msgArgs );
                va_end( msgArgs );
            }

        protected:
            Exception() throw() {} // hide default c_tor
        };

        class FileInUseException : public RCS::Exception
        {
        public:
            FileInUseException( const tchar_t *path, const tchar_t *username ) : Exception( TXT( "File '%s' is currently in use by '%s'." ), path, username) {}
        };

        class FileOutOfDateException : public RCS::Exception
        {
        public:
            FileOutOfDateException( const tchar_t *path, const int curRev, const int headRev ) : Exception( TXT( "File '%s' is not up to date (local revision: %d / remote revision: %d).  Please sync the file to resolve this error." ), path, curRev, headRev ) {}
        };
    }
}