#pragma once

#include "Platform/Types.h"
#include "Foundation/API.h"

namespace Helium
{
    /// Default integer hash function.
    template< typename T >
    class Hash
    {
    public:
        size_t operator()( const T& rKey ) const;
    };

    /// Default pointer hash.
    template< typename T >
    class Hash< T* >
    {
    public:
        size_t operator()( const T* pKey ) const;
    };

    /// Default pointer hash.
    template< typename T >
    class Hash< const T* >
    {
    public:
        size_t operator()( const T* pKey ) const;
    };

#if HELIUM_WORDSIZE == 32
    /// Default 64-bit signed integer hash function.
    template<>
    class HELIUM_FOUNDATION_API Hash< int64_t >
    {
    public:
        inline size_t operator()( int64_t value ) const;
    };

    /// Default 64-bit unsigned integer hash function.
    template<>
    class HELIUM_FOUNDATION_API Hash< uint64_t >
    {
    public:
        inline size_t operator()( uint64_t value ) const;
    };
#endif

    /// Default 32-bit floating-point hash function.
    template<>
    class HELIUM_FOUNDATION_API Hash< float32_t >
    {
    public:
        inline size_t operator()( float32_t key ) const;
    };

    /// Default 64-bit floating-point hash function.
    template<>
    class HELIUM_FOUNDATION_API Hash< float64_t >
    {
    public:
        inline size_t operator()( float64_t key ) const;
    };

    /// Default string hash.
    template<>
    class HELIUM_FOUNDATION_API Hash< char* >
    {
    public:
        size_t operator()( const char* pKey ) const;
    };

    /// Default wide-character string hash.
    template<>
    class HELIUM_FOUNDATION_API Hash< wchar_t* >
    {
    public:
        size_t operator()( const wchar_t* pKey ) const;
    };

    /// Default string hash.
    template<>
    class HELIUM_FOUNDATION_API Hash< const char* >
    {
    public:
        size_t operator()( const char* pKey ) const;
    };

    /// Default wide-character string hash.
    template<>
    class HELIUM_FOUNDATION_API Hash< const wchar_t* >
    {
    public:
        size_t operator()( const wchar_t* pKey ) const;
    };
}

#include "Foundation/HashFunctions.inl"
