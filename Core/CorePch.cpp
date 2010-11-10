//----------------------------------------------------------------------------------------------------------------------
// CorePch.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

// Define the memory heap for the current module and include the "new"/"delete" operator implementations.
L_DEFINE_DEFAULT_MODULE_HEAP( Core );

#if L_SHARED
#include "Platform/NewDelete.h"
#endif

// If not building with exception support, we need to provide our own implementation for boost::throw_exception().
#if !L_DEBUG
namespace boost
{
    LUNAR_CORE_API void throw_exception( const std::exception& /*rException*/ )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "boost::throw_exception() triggered" ) );
    }
}
#endif  // !L_DEBUG
