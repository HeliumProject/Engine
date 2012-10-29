#pragma once

#include "Platform/Exception.h"

namespace Helium
{
    namespace Component
    {
        class InvalidComponentCollectionException : public Helium::Exception
        {
        public:
            InvalidComponentCollectionException() : Exception( TXT( "Trying to operate on an invalid attribute collection!" ) ) {}
        };

        class MissingComponentException : public Helium::Exception
        {
        public:
            MissingComponentException( const tchar_t* typeName ) : Exception( TXT( "Trying to access a non-existent attribute of type '%s'" ), typeName ) {}
        };

        class DisabledComponentException : public Helium::Exception
        {
        public:
            DisabledComponentException( const tchar_t* typeName ) : Exception( TXT( "Trying to access a disabled attribute of type '%s'" ), typeName ) {}
        };
    }
}