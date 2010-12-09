//----------------------------------------------------------------------------------------------------------------------
// AnimationResourceHandler.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"
#include "EditorSupport/AnimationResourceHandler.h"

#include "Foundation/StringConverter.h"
#include "Engine/BinarySerializer.h"
#include "Graphics/Animation.h"
#include "PcSupport/ObjectPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"
#include "EditorSupport/FbxSupport.h"

#if L_USE_GRANNY_ANIMATION
#include "GrannyAnimationResourceHandlerInterface.h"
#include "EditorSupport/GrannyStreamWriter.h"
#endif

using namespace Lunar;

L_IMPLEMENT_OBJECT( AnimationResourceHandler, EditorSupport, 0 );

/// Constructor.
AnimationResourceHandler::AnimationResourceHandler()
: m_rFbxSupport( FbxSupport::StaticAcquire() )
{
}

/// Destructor.
AnimationResourceHandler::~AnimationResourceHandler()
{
    m_rFbxSupport.Release();
}

/// @copydoc ResourceHandler::GetResourceType()
GameObjectType* AnimationResourceHandler::GetResourceType() const
{
    return Animation::GetStaticType();
}

/// @copydoc ResourceHandler::GetSourceExtensions()
void AnimationResourceHandler::GetSourceExtensions(
    const tchar_t* const*& rppExtensions,
    size_t& rExtensionCount ) const
{
    static const tchar_t* extensions[] = { TXT( "_anim.fbx" ) };

    rppExtensions = extensions;
    rExtensionCount = HELIUM_ARRAY_COUNT( extensions );
}

/// @copydoc ResourceHandler::CacheResource()
bool AnimationResourceHandler::CacheResource(
    ObjectPreprocessor* pObjectPreprocessor,
    Resource* pResource,
    const String& rSourceFilePath )
{
    HELIUM_ASSERT( pObjectPreprocessor );
    HELIUM_ASSERT( pResource );

#if L_USE_GRANNY_ANIMATION
    Animation* pAnimation = StaticCast< Animation >( pResource );

    bool bCacheResult = Granny::CacheAnimationResourceData(
        pObjectPreprocessor,
        pAnimation,
        rSourceFilePath,
        m_rFbxSupport );

    return bCacheResult;
#else
    HELIUM_UNREF( pObjectPreprocessor );
    HELIUM_UNREF( rSourceFilePath );

    for( size_t platformIndex = 0; platformIndex < static_cast< size_t >( Cache::PLATFORM_MAX ); ++platformIndex )
    {
        Resource::PreprocessedData& rPreprocessedData = pResource->GetPreprocessedData(
            static_cast< Cache::EPlatform >( platformIndex ) );
        rPreprocessedData.persistentDataBuffer.Clear();
        rPreprocessedData.subDataBuffers.Clear();
        rPreprocessedData.bLoaded = true;
    }

    return true;
#endif
}
