//----------------------------------------------------------------------------------------------------------------------
// DynamicLibrary.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Retrieve the address of the function with the specified name in this library.
    ///
    /// @param[in] rFunctionName  Name of the function to locate.
    ///
    /// @return  Pointer to the function address if the function was found, null if the function was not found.
    void* DynamicLibrary::GetFunctionAddress( const CharString& rFunctionName ) const
    {
        return GetFunctionAddress( *rFunctionName );
    }

    /// Load a library.
    ///
    /// @param[in] rPath  Path of the library file to load.
    ///
    /// @return  Interface to the library if loaded successfully, null if loading failed.  The library is unloaded by
    ///          destroying the returned interface.
    DynamicLibrary* DynamicLibrary::Load( const String& rPath )
    {
        return Load( *rPath );
    }
}
