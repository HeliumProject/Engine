//----------------------------------------------------------------------------------------------------------------------
// Functions.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_FUNCTIONS_H
#define LUNAR_CORE_FUNCTIONS_H

#include "Core/Core.h"

namespace Lunar
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
    class LUNAR_CORE_API Equals< char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "equals" function.
    template<>
    class LUNAR_CORE_API Equals< wchar_t* >
    {
    public:
        bool operator()( const wchar_t* pA, const wchar_t* pB ) const;
    };

    /// String "equals" function.
    template<>
    class LUNAR_CORE_API Equals< const char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "equals" function.
    template<>
    class LUNAR_CORE_API Equals< const wchar_t* >
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
    class LUNAR_CORE_API Less< char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "less-than" function.
    template<>
    class LUNAR_CORE_API Less< wchar_t* >
    {
    public:
        bool operator()( const wchar_t* pA, const wchar_t* pB ) const;
    };

    /// String "less-than" function.
    template<>
    class LUNAR_CORE_API Less< const char* >
    {
    public:
        bool operator()( const char* pA, const char* pB ) const;
    };

    /// Wide-character string "less-than" function.
    template<>
    class LUNAR_CORE_API Less< const wchar_t* >
    {
    public:
        bool operator()( const wchar_t* pA, const wchar_t* pB ) const;
    };
}

#include "Core/Functions.inl"

#endif  // LUNAR_CORE_FUNCTIONS_H
