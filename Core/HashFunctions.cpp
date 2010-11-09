//----------------------------------------------------------------------------------------------------------------------
// HashFunctions.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/HashFunctions.h"

#include "Platform/Utility.h"

namespace Lunar
{
    /// Default string hash.
    ///
    /// @param[in] pKey  Key for which to compute a hash value.
    ///
    /// @return  Hash value.
    size_t Hash< char* >::operator()( const char* pKey ) const
    {
        return StringHash( pKey );
    }

    /// Default wide-character string hash.
    ///
    /// @param[in] pKey  Key for which to compute a hash value.
    ///
    /// @return  Hash value.
    size_t Hash< wchar_t* >::operator()( const wchar_t* pKey ) const
    {
        return StringHash( pKey );
    }

    /// Default string hash.
    ///
    /// @param[in] pKey  Key for which to compute a hash value.
    ///
    /// @return  Hash value.
    size_t Hash< const char* >::operator()( const char* pKey ) const
    {
        return StringHash( pKey );
    }

    /// Default wide-character string hash.
    ///
    /// @param[in] pKey  Key for which to compute a hash value.
    ///
    /// @return  Hash value.
    size_t Hash< const wchar_t* >::operator()( const wchar_t* pKey ) const
    {
        return StringHash( pKey );
    }
}
