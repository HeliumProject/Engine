#pragma once

#include "Foundation/String.h"

namespace Helium
{
    /// String conversion base template.
    ///
    /// This provides no functionality by itself.  Only the fully specialized versions can be used directly.
    template< typename SourceCharType, typename DestCharType >
    class StringConverter
    {
    };

    /// Single-byte character string to wide string conversion.
    ///
    /// This performs a conversion based on the current locale settings.  As such, characters outside the standard 7-bit
    /// ASCII character set may be converted differently based on the platform on which the application is currently
    /// running.
    template<>
    class StringConverter< char, wchar_t >
    {
    public:
        /// @name Conversion Functions
        //@{
        inline static size_t Convert( wchar_t* pDestString, size_t destBufferSize, const char* pSourceString );
        template< typename SourceAllocator > static size_t Convert(
            wchar_t* pDestString, size_t destBufferSize, const StringBase< char, SourceAllocator >& rSourceString );
        template< typename DestAllocator > static bool Convert(
            StringBase< wchar_t, DestAllocator >& rDestString, const char* pSourceString );
        template< typename DestAllocator, typename SourceAllocator > static bool Convert(
            StringBase< wchar_t, DestAllocator >& rDestString,
            const StringBase< char, SourceAllocator >& rSourceString );
        //@}
    };

    /// Wide string to single-byte character string conversion.
    ///
    /// This performs a conversion based on the current locale settings.  As such, it may not be possible to convert
    /// certain characters outside the standard 7-bit ASCII character set due to restrictions with the current locale's
    /// character support.
    template<>
    class StringConverter< wchar_t, char >
    {
    public:
        /// @name Conversion Functions
        //@{
        inline static size_t Convert( char* pDestString, size_t destBufferSize, const wchar_t* pSourceString );
        template< typename SourceAllocator > static size_t Convert(
            char* pDestString, size_t destBufferSize, const StringBase< wchar_t, SourceAllocator >& rSourceString );
        template< typename DestAllocator > static bool Convert(
            StringBase< char, DestAllocator >& rDestString, const wchar_t* pSourceString );
        template< typename DestAllocator, typename SourceAllocator > static bool Convert(
            StringBase< char, DestAllocator >& rDestString,
            const StringBase< wchar_t, SourceAllocator >& rSourceString );
        //@}
    };

    /// Null string conversion.
    ///
    /// This is a stub for converting to and from the same character type.
    template< typename CharType >
    class NullStringConverter
    {
    public:
        /// @name Conversion Functions
        //@{
        inline static size_t Convert( CharType* pDestString, size_t destBufferSize, const CharType* pSourceString );
        template< typename SourceAllocator > static size_t Convert(
            CharType* pDestString, size_t destBufferSize,
            const StringBase< CharType, SourceAllocator >& rSourceString );
        template< typename DestAllocator > static bool Convert(
            StringBase< CharType, DestAllocator >& rDestString, const CharType* pSourceString );
        template< typename DestAllocator, typename SourceAllocator > static bool Convert(
            StringBase< CharType, DestAllocator >& rDestString,
            const StringBase< CharType, SourceAllocator >& rSourceString );
        //@}
    };

    /// Null single-byte character string conversion.
    template<>
    class StringConverter< char, char > : public NullStringConverter< char >
    {
    };

    /// Null wide character string conversion.
    template<>
    class StringConverter< wchar_t, wchar_t > : public NullStringConverter< wchar_t >
    {
    };
}

#include "Foundation/StringConverter.inl"
