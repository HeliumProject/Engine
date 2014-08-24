#include "PcSupportPch.h"
#include "PcSupport/ResourceHandler.h"

HELIUM_IMPLEMENT_ASSET( Helium::ResourceHandler, PcSupport, 0 );

using namespace Helium;

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
const AssetType* ResourceHandler::GetResourceType() const
{
    return NULL;
}

/// Get the list of source file extensions handled by this resource type.
///
/// @param[out] rppExtensions    Array of file extensions for supported file types.
/// @param[out] rExtensionCount  Number of file extensions in the given array.
void ResourceHandler::GetSourceExtensions( const char* const*& rppExtensions, size_t& rExtensionCount ) const
{
    rppExtensions = NULL;
    rExtensionCount = 0;
}

#if HELIUM_TOOLS
/// Preprocess and cache the resource data for the given resource for all enabled target platforms.
///
/// @param[in] pAssetPreprocessor  Asset preprocessor instance.
/// @param[in] pResource            Resource object.
/// @param[in] rSourceFilePath      Prebuilt path name of the source resource file.
///
/// @return  True if the resource data was loaded and cached successfully, false if not.
bool ResourceHandler::CacheResource(
                                    AssetPreprocessor* /*pAssetPreprocessor*/,
                                    Resource* /*pResource*/,
                                    const String& /*rSourceFilePath*/ )
{
    return false;
}
#endif  // HELIUM_TOOLS


#if HELIUM_TOOLS
void Helium::ResourceHandler::SaveObjectToPersistentDataBuffer( Reflect::Object *_object, DynamicArray< uint8_t > &_buffer )
{
    _buffer.Resize(0);
    if (!_object)
    {
        return;
    }

    Cache::WriteCacheObjectToBuffer(_object, _buffer);
}


#endif  // HELIUM_TOOLS

/// Gather all existing resource handlers.
///
/// This will iterate through all sub-types of ResourceHandler and add their template objects to a dynamic array.
///
/// @param[out] rResourceHandlers  List of existing resource handlers.
void ResourceHandler::GetAllResourceHandlers( DynamicArray< ResourceHandler* >& rResourceHandlers )
{
    rResourceHandlers.Resize( 0 );

    const AssetType* pResourceHandlerType = GetStaticType();
    HELIUM_ASSERT( pResourceHandlerType );

    AssetType::ConstIterator typeEnd = AssetType::GetTypeEnd();
    for( AssetType::ConstIterator typeIterator = AssetType::GetTypeBegin(); typeIterator != typeEnd; ++typeIterator )
    {
        const AssetType& rType = *typeIterator;
        if( &rType != pResourceHandlerType && rType.GetMetaClass()->IsType( pResourceHandlerType->GetMetaClass() ) )
        {
            ResourceHandler* pHandler = Reflect::AssertCast< ResourceHandler >( rType.GetTemplate() );
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
ResourceHandler* ResourceHandler::FindResourceHandlerForType( const AssetType* pType )
{
    HELIUM_ASSERT( pType );

    const AssetType* pResourceHandlerType = GetStaticType();
    HELIUM_ASSERT( pResourceHandlerType );

    AssetType::ConstIterator typeEnd = AssetType::GetTypeEnd();
    for( AssetType::ConstIterator typeIterator = AssetType::GetTypeBegin(); typeIterator != typeEnd; ++typeIterator )
    {
        const AssetType& rType = *typeIterator;
        if( &rType != pResourceHandlerType && rType.GetMetaClass()->IsType( pResourceHandlerType->GetMetaClass() ) )
        {
            ResourceHandler* pHandler = Reflect::AssertCast< ResourceHandler >( rType.GetTemplate() );
            HELIUM_ASSERT( pHandler );
            if( pHandler->GetResourceType() == pType )
            {
                return pHandler;
            }
        }
    }

    return NULL;
}

ResourceHandler *ResourceHandler::GetBestResourceHandlerForFile( const String &fileName )
{
	ResourceHandler* pBestHandler = NULL;
	size_t bestHandlerExtensionLength = 0;

	// Add all resource objects that exist in the package directory.
	DynamicArray< ResourceHandler* > resourceHandlers;
	ResourceHandler::GetAllResourceHandlers( resourceHandlers );
	size_t resourceHandlerCount = resourceHandlers.GetSize();

#if HELIUM_TOOLS
	// The tools build needs to load the resource handlers!
	HELIUM_ASSERT( resourceHandlerCount );
#endif

	for( size_t handlerIndex = 0; handlerIndex < resourceHandlerCount; ++handlerIndex )
	{
		ResourceHandler* pHandler = resourceHandlers[ handlerIndex ];
		HELIUM_ASSERT( pHandler );

		const char* const* ppExtensions;
		size_t extensionCount;
		pHandler->GetSourceExtensions( ppExtensions, extensionCount );
		HELIUM_ASSERT( ppExtensions || extensionCount == 0 );

		for( size_t extensionIndex = 0; extensionIndex < extensionCount; ++extensionIndex )
		{
			const char* pExtension = ppExtensions[ extensionIndex ];
			HELIUM_ASSERT( pExtension );

			size_t extensionLength = StringLength( pExtension );
			if( extensionLength > bestHandlerExtensionLength && fileName.EndsWith( pExtension ) )
			{
				pBestHandler = pHandler;
				bestHandlerExtensionLength = extensionLength;

				break;
			}
		}
	}

	return pBestHandler;
}