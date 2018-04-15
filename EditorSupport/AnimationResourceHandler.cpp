#include "Precompile.h"

#if HELIUM_TOOLS

#include "EditorSupport/AnimationResourceHandler.h"

#include "Foundation/StringConverter.h"
#include "Graphics/Animation.h"
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"
#include "EditorSupport/FbxSupport.h"

#if HELIUM_USE_GRANNY_ANIMATION
#include "GrannyAnimationResourceHandlerInterface.h"
#include "EditorSupport/GrannyStreamWriter.h"
#endif

HELIUM_IMPLEMENT_ASSET( Helium::AnimationResourceHandler, EditorSupport, 0 );

using namespace Helium;

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
const AssetType* AnimationResourceHandler::GetResourceType() const
{
    return Animation::GetStaticType();
}

/// @copydoc ResourceHandler::GetSourceExtensions()
void AnimationResourceHandler::GetSourceExtensions(
    const char* const*& rppExtensions,
    size_t& rExtensionCount ) const
{
    static const char* extensions[] = { "_anim.fbx" };

    rppExtensions = extensions;
    rExtensionCount = HELIUM_ARRAY_COUNT( extensions );
}

/// @copydoc ResourceHandler::CacheResource()
bool AnimationResourceHandler::CacheResource(
    AssetPreprocessor* pAssetPreprocessor,
    Resource* pResource,
    const String& rSourceFilePath )
{
    HELIUM_ASSERT( pAssetPreprocessor );
    HELIUM_ASSERT( pResource );

#if HELIUM_USE_GRANNY_ANIMATION
    Animation* pAnimation = Reflect::AssertCast< Animation >( pResource );

    bool bCacheResult = Granny::CacheAnimationResourceData(
        pAssetPreprocessor,
        pAnimation,
        rSourceFilePath,
        m_rFbxSupport );

    return bCacheResult;
#else
    HELIUM_UNREF( pAssetPreprocessor );
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

#endif  // HELIUM_TOOLS
