//----------------------------------------------------------------------------------------------------------------------
// ResourceHandler.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "PcSupportPch.h"
#include "PcSupport/ResourceHandler.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( ResourceHandler, PcSupport, 0 );

/// Constructor.
ResourceHandler::ResourceHandler()
{
}

/// Destructor.
ResourceHandler::~ResourceHandler()
{
}

/// Get the type of resource associated with this handler.
///
/// @return  Resource type.
const GameObjectType* ResourceHandler::GetResourceType() const
{
    return NULL;
}

/// Get the list of source file extensions handled by this resource type.
///
/// @param[out] rppExtensions    Array of file extensions for supported file types.
/// @param[out] rExtensionCount  Number of file extensions in the given array.
void ResourceHandler::GetSourceExtensions( const tchar_t* const*& rppExtensions, size_t& rExtensionCount ) const
{
    rppExtensions = NULL;
    rExtensionCount = 0;
}

#if L_EDITOR
/// Preprocess and cache the resource data for the given resource for all enabled target platforms.
///
/// @param[in] pObjectPreprocessor  GameObject preprocessor instance.
/// @param[in] pResource            Resource object.
/// @param[in] rSourceFilePath      Prebuilt path name of the source resource file.
///
/// @return  True if the resource data was loaded and cached successfully, false if not.
bool ResourceHandler::CacheResource(
                                    ObjectPreprocessor* /*pObjectPreprocessor*/,
                                    Resource* /*pResource*/,
                                    const String& /*rSourceFilePath*/ )
{
    return false;
}
#endif  // L_EDITOR

/// Gather all existing resource handlers.
///
/// This will iterate through all sub-types of ResourceHandler and add their template objects to a dynamic array.
///
/// @param[out] rResourceHandlers  List of existing resource handlers.
void ResourceHandler::GetAllResourceHandlers( DynArray< ResourceHandler* >& rResourceHandlers )
{
    rResourceHandlers.Resize( 0 );

    const GameObjectType* pResourceHandlerType = GetStaticType();
    HELIUM_ASSERT( pResourceHandlerType );

    GameObjectType::ConstIterator typeEnd = GameObjectType::GetTypeEnd();
    for( GameObjectType::ConstIterator typeIterator = GameObjectType::GetTypeBegin(); typeIterator != typeEnd; ++typeIterator )
    {
        const GameObjectType& rType = *typeIterator;
        if( &rType != pResourceHandlerType && rType.IsSubtypeOf( pResourceHandlerType ) )
        {
            ResourceHandler* pHandler = StaticCast< ResourceHandler >( rType.GetTemplate() );
            HELIUM_ASSERT( pHandler );
            rResourceHandlers.Push( pHandler );
        }
    }
}

/// Search for the resource handler for the specified type.
///
/// @param[in] pType  Resource type.
///
/// @return  Resource handler for the given type if found, null if no resource handler was found.
ResourceHandler* ResourceHandler::FindResourceHandlerForType( const GameObjectType* pType )
{
    HELIUM_ASSERT( pType );

    const GameObjectType* pResourceHandlerType = GetStaticType();
    HELIUM_ASSERT( pResourceHandlerType );

    GameObjectType::ConstIterator typeEnd = GameObjectType::GetTypeEnd();
    for( GameObjectType::ConstIterator typeIterator = GameObjectType::GetTypeBegin(); typeIterator != typeEnd; ++typeIterator )
    {
        const GameObjectType& rType = *typeIterator;
        if( &rType != pResourceHandlerType && rType.IsSubtypeOf( pResourceHandlerType ) )
        {
            ResourceHandler* pHandler = StaticCast< ResourceHandler >( rType.GetTemplate() );
            HELIUM_ASSERT( pHandler );
            if( pHandler->GetResourceType() == pType )
            {
                return pHandler;
            }
        }
    }

    return NULL;
}
