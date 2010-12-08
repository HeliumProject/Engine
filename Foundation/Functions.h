#pragma once

#include "Foundation/API.h"

namespace Helium
{
    /// Default "equals" function.
    template< typename T >
    class Equals
    {
    public:
        bool operator()( const T& rA, const T& rB ) const;
    };

    /// String "equals" function.
    template<>
    class FOUNDATION_API Equals< char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "equals" function.
    template<>
    class FOUNDATION_API Equals< wchar_t* >
    {
    public:
        bool operator()( const wchar_t* pA, const wchar_t* pB ) const;
    };

    /// String "equals" function.
    template<>
    class FOUNDATION_API Equals< const char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "equals" function.
    template<>
    class FOUNDATION_API Equals< const wchar_t* >
    {
    public:
        bool operator()( const wchar_t* pA, const wchar_t* pB ) const;
    };

    /// Default "less-than" function.
    template< typename T >
    class Less
    {
    public:
        bool operator()( const T& rA, const T& rB ) const;
    };

    /// String "less-than" function.
    template<>
    class FOUNDATION_API Less< char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "less-than" function.
    template<>
    class FOUNDATION_API Less< wchar_t* >
    {
    public:
        bool operator()( const wchar_t* pA, const wchar_t* pB ) const;
    };

    /// String "less-than" function.
    template<>
    class FOUNDATION_API Less< const char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "less-than" function.
    template<>
    class FOUNDATION_API Less< const wchar_t* >
    {
    public:
        bool operator()( const wchar_t* pA, const wchar_t* pB ) const;
    };

    /// Identity value selection function.
    template< typename T >
    class Identity
    {
    public:
        T& operator()( T& rValue ) const;
        const T& operator()( const T& rValue ) const;
    };
}

#include "Foundation/Functions.inl"
