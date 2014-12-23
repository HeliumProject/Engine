#include "EnginePch.h"
#include "Engine/PipelinePackageLoader.h"

#include "Engine/PipelineClient.h"

using namespace Helium;


Helium::PipelinePackageLoader::PipelinePackageLoader()
: m_AssetInfoReceived(false)
, m_loadRequestPool( LOAD_REQUEST_POOL_BLOCK_SIZE )
, m_parentPackageLoadId( Invalid< size_t >() )
, m_startPreloadCounter( 0 )
, m_preloadedCounter( 0 )
{

}

Helium::PipelinePackageLoader::~PipelinePackageLoader()
{
	Shutdown();
}

bool Helium::PipelinePackageLoader::Initialize( AssetPath packagePath )
{
	// TODO: We do the same thing in LoosePackageLoader...
	// Make sure the path represents a package.
	if( packagePath.IsEmpty() )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "PipelinePackageLoader::Initialize(): Empty package path specified.\n" ) );

		return false;
	}

	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT( "PipelinePackageLoader::Initialize(): Initializing loader for package \"%s\".\n" ),
		*packagePath.ToString() );

	if( !packagePath.IsPackage() )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "PipelinePackageLoader::Initialize(): \"%s\" does not represent a package path.\n" ),
			*packagePath.ToString() );

		return false;
	}

	// Store the package path.
	m_PackagePath = packagePath;

	// TODO: We do the same thing in LoosePackageLoader...
	// Attempt to locate the specified package if it already happens to exist.
	m_Package = Asset::Find< Package >( packagePath );
	Package* pPackage = m_Package;
	if( pPackage )
	{
		if( pPackage->GetLoader() )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "PipelinePackageLoader::Initialize(): Package \"%s\" already has a loader.\n" ),
				*packagePath.ToString() );

			m_Package.Release();

			return false;
		}

		pPackage->SetLoader( this );
	}
	else
	{
		// Make sure we don't have a name clash with a non-package object.
		AssetPtr assetPtr( Asset::FindObject( packagePath ) );
		if( assetPtr )
		{
			HELIUM_ASSERT( !assetPtr->IsPackage() );

			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "PackageLoader::Initialize(): Package loader cannot be initialized for \"%s\", as an " )
				TXT( "object with the same name exists that is not a package.\n" ) ),
				*packagePath.ToString() );

			return false;
		}
	}

	PipelineClient::GetStaticInstance()->SubscribePackage(m_PackagePath);

	return true;
}

void Helium::PipelinePackageLoader::Shutdown()
{	
	// Sync with any in-flight async load requests.
	if( m_startPreloadCounter )
	{
		while( !TryFinishPreload() )
		{
			Tick();
		}
	}

	HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );

	// Unset the reference back to this loader in the package.
	Package* pPackage = m_Package;
	if( pPackage )
	{
		pPackage->SetLoader( NULL );
	}

	m_Package.Release();
	m_PackagePath.Clear();

	AtomicExchangeRelease( m_startPreloadCounter, 0 );
	AtomicExchangeRelease( m_preloadedCounter, 0 );

	m_Metadata.Clear();

	size_t loadRequestCount = m_loadRequests.GetSize();
	for( size_t requestIndex = 0; requestIndex < loadRequestCount; ++requestIndex )
	{
		if( m_loadRequests.IsElementValid( requestIndex ) )
		{
			LoadRequest* pRequest = m_loadRequests[ requestIndex ];
			HELIUM_ASSERT( pRequest );
			m_loadRequestPool.Release( pRequest );
		}
	}

	m_loadRequests.Clear();

	PipelineClient::GetStaticInstance()->UnsubscribePackage(m_PackagePath);
}

bool Helium::PipelinePackageLoader::BeginPreload()
{	
	HELIUM_ASSERT( !m_startPreloadCounter );
	HELIUM_ASSERT( !m_preloadedCounter );
	HELIUM_ASSERT( IsInvalid( m_parentPackageLoadId ) );

	// Load the parent package if we need to create the current package.
	if( !m_Package )
	{
		AssetPath parentPackagePath = m_PackagePath.GetParent();
		if( !parentPackagePath.IsEmpty() )
		{
			AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
			HELIUM_ASSERT( pAssetLoader );

			m_parentPackageLoadId = pAssetLoader->BeginLoadObject( parentPackagePath );
			HELIUM_ASSERT( IsValid( m_parentPackageLoadId ) );
		}
	}

	AtomicExchangeRelease( m_startPreloadCounter, 1 );

	return true;
}

bool Helium::PipelinePackageLoader::TryFinishPreload()
{
	return ( m_preloadedCounter != 0 );
}

size_t Helium::PipelinePackageLoader::BeginLoadObject( AssetPath path, Reflect::ObjectResolver *pResolver, bool forceReload /*= false */ )
{	
	HELIUM_TRACE( TraceLevels::Info, TXT(" PipelinePackageLoader::BeginLoadObject - Loading path %s\n"), *path.ToString() );

	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT( "PipelinePackageLoader::BeginLoadObject: Beginning load for path \"%s\".\n"),
		*path.ToString());

	// Check that we have finished preloading
	if (!m_AssetInfoReceived)
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			TXT( "PipelinePackageLoader::BeginLoadObject: Path \"%s\" failed, the package is not preloaded yet!.\n"),
			*path.ToString());

		return Invalid<size_t>();
	}

	// Make sure preloading has completed.
	HELIUM_ASSERT( m_preloadedCounter != 0 );
	if( !m_preloadedCounter )
	{
		return Invalid< size_t >();
	}

	// If the request is for our package, then just return it
	if (path == m_PackagePath)
	{
		HELIUM_TRACE(
			TraceLevels::Debug,
			TXT( "PipelinePackageLoader::BeginLoadObject: Path \"%s\" is the package itself. Load is instantly successful.\n"),
			*path.ToString());

		LoadRequest *pRequest = AllocateLoadRequest();

		HELIUM_ASSERT( m_Package );
		pRequest->m_Asset = m_Package;
		pRequest->m_Flags = LOAD_FLAG_PRELOADED;

		size_t requestId = m_loadRequests.Add( pRequest );
		return requestId;
	}

	size_t objectIndex = FindObjectByPath( path );

	if ( !IsValid( objectIndex ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "BeginLoadObject::BeginLoadObject(): Failed to locate \"%s\" for loading. Verify the asset exists.\n" ),
			*path.ToString() );

		return Invalid< size_t >();
	}

	AssetMetadata& metadata = m_Metadata[ objectIndex ];

	// Locate the type object.
	HELIUM_ASSERT( !metadata.m_TypeName.IsEmpty() );
	AssetType* pType = AssetType::Find( metadata.m_TypeName );
	if( !pType )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "BeginLoadObject::BeginLoadObject(): Failed to locate type \"%s\" for loading object \"%s\".\n" ),
			*metadata.m_TypeName,
			*path.ToString() );

		HELIUM_TRACE(
			TraceLevels::Debug,
			TXT( "Current registered types:\n" ) );

		for ( AssetType::ConstIterator iter = AssetType::GetTypeBegin();
			iter != AssetType::GetTypeEnd(); ++iter)
		{
			HELIUM_TRACE(
				TraceLevels::Debug,
				TXT( " - %s\n" ),
				*iter->GetName() );
		}

		return Invalid< size_t >();
	}

	// This makes sure we aren't trying to load the same object twice
#ifndef NDEBUG
	size_t loadRequestSize = m_loadRequests.GetSize();
	for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestSize; ++loadRequestIndex )
	{
		if( !m_loadRequests.IsElementValid( loadRequestIndex ) )
		{
			continue;
		}

		LoadRequest* pRequest = m_loadRequests[ loadRequestIndex ];
		HELIUM_ASSERT( pRequest );
		HELIUM_ASSERT( pRequest->m_Index != objectIndex );
		if( pRequest->m_Index == objectIndex )
		{
			return Invalid< size_t >();
		}
	}
#endif

	LoadRequest* pRequest = AllocateLoadRequest();
	pRequest->m_Index = objectIndex;
	pRequest->m_pResolver = pResolver;
	pRequest->m_Type = pType;

	// If this asset is already loaded, we can just bail now
	if ( !forceReload )
	{
		pRequest->m_Asset = Asset::FindObject( path );
		Asset* pAsset = pRequest->m_Asset;
		if( pAsset && pAsset->IsFullyLoaded() )
		{
			HELIUM_TRACE(
				TraceLevels::Debug,
				TXT( "PipelinePackageLoader::BeginLoadObject: Path \"%s\" already loaded. Load is instantly successful.\n"),
				*path.ToString());

			pRequest->m_Flags = LOAD_FLAG_PRELOADED;
			size_t requestId = m_loadRequests.Add( pRequest );
			return requestId;
		}
	}

	Asset* pAsset = pRequest->m_Asset;
	HELIUM_ASSERT( !pAsset || !pAsset->GetAnyFlagSet( Asset::FLAG_LOADED | Asset::FLAG_LINKED ) );

	// Begin loading the template and owner objects.  Note that there isn't much reason to check for failure
	// until we tick this request, as we need to make sure any other load requests for the template/owner that
	// did succeed are properly synced anyway.
	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	if( metadata.m_TemplatePath.IsEmpty() )
	{
		// Make sure the template is fully loaded.
		Asset* pTemplate = pType->GetTemplate();
		metadata.m_TemplatePath = pTemplate->GetPath();
		if( pTemplate->IsFullyLoaded() )
		{
			pRequest->m_Template = pTemplate;
		}
		else
		{
			pRequest->m_TemplateLoadId = pAssetLoader->BeginLoadObject( metadata.m_TemplatePath );
		}
	}
	else
	{
		pRequest->m_TemplateLoadId = pAssetLoader->BeginLoadObject( metadata.m_TemplatePath );
	}

	AssetPath ownerPath = path.GetParent();
	if( ownerPath == m_PackagePath )
	{
		// Easy check: if the owner is this package (which is likely), we don't need to load it.
		pRequest->m_Owner = m_Package;
	}
	else if( !ownerPath.IsEmpty() )
	{
		pRequest->m_OwnerLoadId = pAssetLoader->BeginLoadObject( ownerPath );
	}

	size_t requestId = m_loadRequests.Add( pRequest );

	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT( "PipelinePackageLoader::BeginLoadObject: Path \"%s\" is loading. Template: \"%s\" Owner: \"%s\"\n"),
		*path.ToString(),
		*metadata.m_TemplatePath.ToString(),
		*ownerPath.ToString());

	return requestId;
}

bool Helium::PipelinePackageLoader::TryFinishLoadObject( size_t requestId, AssetPtr& assetPtr )
{
	HELIUM_ASSERT( requestId < m_loadRequests.GetSize() );
	HELIUM_ASSERT( m_loadRequests.IsElementValid( requestId ) );

	LoadRequest* pRequest = m_loadRequests[ requestId ];
	HELIUM_ASSERT( pRequest );

	if ( !pRequest->IsPreloaded() )
	{
		return false;
	}

	HELIUM_ASSERT ( !IsValid( pRequest->m_TemplateLoadId ) );
	HELIUM_ASSERT ( !IsValid( pRequest->m_OwnerLoadId ) );

	assetPtr = pRequest->m_Asset;
	if( assetPtr && pRequest->HasError() )
	{
		assetPtr->SetFlags( Asset::FLAG_BROKEN );
	}

	pRequest->m_pResolver = NULL;
	pRequest->m_Asset.Release();
	pRequest->m_Template.Release();
	pRequest->m_Owner.Release();
	pRequest->m_Type.Release();

	m_loadRequests.Remove( requestId );
	m_loadRequestPool.Release( pRequest );

	return true;
}

void Helium::PipelinePackageLoader::Tick()
{
	MutexScopeLock scopeLock( m_accessLock );

	// Do nothing until pre-loading has been started.
	if( !m_startPreloadCounter )
	{
		return;
	}

	if( !m_preloadedCounter )
	{
		// Update package preloading.
		TickPreload();
	}
	else
	{
		// Process pending dependencies.
		TickLoadRequests();
	}
}

/// Update during the package preload process.
void PipelinePackageLoader::TickPreload()
{
	HELIUM_ASSERT( m_startPreloadCounter != 0 );
	HELIUM_ASSERT( m_preloadedCounter == 0 );

	// Wait for the parent package to finish loading.
	AssetPtr spParentPackage;
	if( IsValid( m_parentPackageLoadId ) )
	{
		AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
		HELIUM_ASSERT( pAssetLoader );
		if( !pAssetLoader->TryFinishLoad( m_parentPackageLoadId, spParentPackage ) )
		{
			return;
		}

		SetInvalid( m_parentPackageLoadId );

		// Package loading should not fail.  If it does, this is a sign of a potentially serious issue.
		HELIUM_ASSERT( spParentPackage );
	}

	// Don't allow preload to finish until we receive asset data
	if (!m_AssetInfoReceived)
	{
		return;
	}

	// Create the package object if it does not yet exist.
	Package* pPackage = m_Package;
	if( !pPackage )
	{
		HELIUM_ASSERT( spParentPackage ? !m_PackagePath.GetParent().IsEmpty() : m_PackagePath.GetParent().IsEmpty() );
		HELIUM_VERIFY( Asset::Create< Package >( m_Package, m_PackagePath.GetName(), spParentPackage ) );
		pPackage = m_Package;
		HELIUM_ASSERT( pPackage );
		pPackage->SetLoader( this );
	}

	HELIUM_ASSERT( pPackage->GetLoader() == this );

	// Package preloading is now complete.
	pPackage->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
	pPackage->ConditionalFinalizeLoad();

	AtomicExchangeRelease( m_preloadedCounter, 1 );
}

/// Update load processing of object load requests.
void PipelinePackageLoader::TickLoadRequests()
{
	size_t loadRequestCount = m_loadRequests.GetSize();
	for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestCount; ++loadRequestIndex )
	{
		if( !m_loadRequests.IsElementValid( loadRequestIndex ) )
		{
			continue;
		}

		LoadRequest* pRequest = m_loadRequests[ loadRequestIndex ];
		HELIUM_ASSERT( pRequest );

#if 0
		if( !( pRequest->m_Flags & LOAD_FLAG_PROPERTY_PRELOADED ) )
		{
			if( !TickDeserialize( pRequest ) )
			{
				continue;
			}
		}

		//TODO: Investigate removing need to preload properties first. Probably need to have the
		//      restriction as TickPersistentResourcePreload assumes the object exists.. but this
		//      may not be the best place to put this 
		// We can probably remove these continues..
		if( !( pRequest->m_Flags & LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED ) )
		{
			if( !TickPersistentResourcePreload( pRequest ) )
			{
				continue;
			}
		}
#endif
	}
}

size_t Helium::PipelinePackageLoader::GetAssetCount() const
{
	return m_Metadata.GetSize();
}

Helium::AssetPath Helium::PipelinePackageLoader::GetAssetPath( size_t index ) const
{
	HELIUM_ASSERT( index < m_Metadata.GetSize() );

	return m_Metadata[ index ].m_Path;
}

Package* Helium::PipelinePackageLoader::GetPackage() const
{
	return m_Package;
}

Helium::AssetPath Helium::PipelinePackageLoader::GetPackagePath() const
{
	return m_PackagePath;
}

#if HELIUM_TOOLS
bool Helium::PipelinePackageLoader::HasAssetFileState() const
{
	return true;
}

size_t Helium::PipelinePackageLoader::GetAssetIndex( const AssetPath &path ) const
{
	return FindObjectByPath( path );
}

const FilePath & Helium::PipelinePackageLoader::GetAssetFileSystemPath( size_t index ) const
{
	return FilePath::NULL_FILE_PATH;
}

int64_t Helium::PipelinePackageLoader::GetAssetFileSystemTimestamp( size_t index ) const
{
	return m_Metadata[ index ].m_Timestamp;
}

Name PipelinePackageLoader::GetAssetTypeName( size_t index ) const
{
	return m_Metadata[ index ].m_TypeName;
}

AssetPath PipelinePackageLoader::GetAssetTemplatePath( size_t index ) const
{
	return m_Metadata[ index ].m_TemplatePath;
}

void Helium::PipelinePackageLoader::EnumerateChildren( DynamicArray< AssetPath > &children ) const
{
	MutexScopeLock scopeLock( m_accessLock );

	for (DynamicArray< AssetPath >::ConstIterator iter = m_ChildPackagePaths.Begin(); 
		iter != m_ChildPackagePaths.End(); ++iter)
	{
		children.Add( *iter );
	}
}

bool Helium::PipelinePackageLoader::SaveAsset( Asset *pAsset ) const
{
	return false;
}
#endif // HELIUM_TOOLS

void Helium::PipelinePackageLoader::UpdateChildPackages( const DynamicArray< String > &childPackages )
{
	MutexScopeLock scopeLock( m_accessLock );

	m_ChildPackagePaths.Clear();

	for ( DynamicArray< String >::ConstIterator iter = childPackages.Begin();
		iter != childPackages.End(); ++iter )
	{
		AssetPath path;
		path.Join( m_PackagePath, **iter );

		m_ChildPackagePaths.Add( path );
	}
}

void Helium::PipelinePackageLoader::UpdateAssetInfo( const Pipeline::PipelineAssetInfo &info )
{
	MutexScopeLock scopeLock( m_accessLock );

	// TODO: Account for child packages
	AssetPath path(*info.m_Path);
	size_t index = FindObjectByPath(path);

	if (index == Invalid<size_t>())
	{
		AssetMetadata *pMetadata = m_Metadata.New();
		pMetadata->m_Path = path;
		pMetadata->m_Size = info.m_Size;
		pMetadata->m_TemplatePath = AssetPath(*info.m_Template);
		pMetadata->m_Timestamp = info.m_Timestamp;
		pMetadata->m_TypeName = Name(info.m_Type);
	}
}

void Helium::PipelinePackageLoader::UpdateAssetInfoComplete()
{
	m_AssetInfoReceived = true;
}

PipelinePackageLoader::LoadRequest *Helium::PipelinePackageLoader::AllocateLoadRequest()
{
	LoadRequest* pRequest = m_loadRequestPool.Allocate();
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !pRequest->m_Asset );
	HELIUM_ASSERT( !pRequest->m_Template );

	pRequest->m_Asset = m_Package;
	SetInvalid( pRequest->m_Index );
	SetInvalid( pRequest->m_TemplateLoadId );
	SetInvalid( pRequest->m_OwnerLoadId );
	pRequest->m_pResolver = NULL;
	pRequest->m_Flags = 0;

	return pRequest;
}

size_t PipelinePackageLoader::FindObjectByPath( const AssetPath &path ) const
{
	// Locate the object within this package.
	size_t objectCount = m_Metadata.GetSize();
	for(size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
	{
		const AssetMetadata& metadata = m_Metadata[ objectIndex ];
		if( metadata.m_Path == path )
		{
			return objectIndex;
		}
	}

	return Invalid<size_t>();
}

size_t PipelinePackageLoader::FindObjectByName( const Name &name ) const
{
	// Locate the object within this package.
	size_t objectCount = m_Metadata.GetSize();
	for(size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
	{
		const AssetMetadata& metadata = m_Metadata[ objectIndex ];
		if( metadata.m_Path.GetName() == name && metadata.m_Path.GetParent() == m_PackagePath )
		{
			return objectIndex;
		}
	}

	return Invalid<size_t>();
}

/// Update processing of object property preloading for a given load request.
///
/// @param[in] pRequest  Load request to process.
///
/// @return  True if object property preloading for the given load request has completed, false if not.
bool PipelinePackageLoader::TickDeserialize( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !pRequest->IsPreloaded() );

	Asset* pAsset = pRequest->m_Asset;

	HELIUM_ASSERT( pRequest->m_Index < m_Metadata.GetSize() );
	AssetMetadata& rObjectData = m_Metadata[ pRequest->m_Index ];

	// Wait for the template and owner objects to load.
	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	if( !rObjectData.m_TemplatePath.IsEmpty() )
	{
		if( IsValid( pRequest->m_TemplateLoadId ) )
		{
			if( !pAssetLoader->TryFinishLoad( pRequest->m_TemplateLoadId, pRequest->m_Template ) )
			{
				return false;
			}

			SetInvalid( pRequest->m_TemplateLoadId );
		}

		if( !pRequest->m_Template )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "PipelinePackageLoader: Failed to load template object for \"%s\".\n" ),
				*rObjectData.m_Path.ToString() );

			if( pAsset )
			{
				pAsset->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
				pAsset->ConditionalFinalizeLoad();
			}

			pRequest->m_Flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}

	HELIUM_ASSERT( IsInvalid( pRequest->m_TemplateLoadId ) );
	Asset* pTemplate = pRequest->m_Template;

	AssetPath ownerPath = rObjectData.m_Path.GetParent();
	if( !ownerPath.IsEmpty() )
	{
		if( IsValid( pRequest->m_OwnerLoadId ) )
		{
			if( !pAssetLoader->TryFinishLoad( pRequest->m_OwnerLoadId, pRequest->m_Owner ) )
			{
				return false;
			}

			SetInvalid( pRequest->m_OwnerLoadId );
		}

		if( !pRequest->m_Owner )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "PipelinePackageLoader: Failed to load owner object for \"%s\".\n" ),
				*rObjectData.m_Path.ToString() );

			if( pAsset )
			{
				pAsset->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
				pAsset->ConditionalFinalizeLoad();
			}

			pRequest->m_Flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;

			return true;
		}
	}

	HELIUM_ASSERT( IsInvalid( pRequest->m_OwnerLoadId ) );
	Asset* pOwner = pRequest->m_Owner;

	AssetType* pType = pRequest->m_Type;
	HELIUM_ASSERT( pType );

	HELIUM_ASSERT( !pOwner || pOwner->IsFullyLoaded() );
	HELIUM_ASSERT( !pTemplate || pTemplate->IsFullyLoaded() );
	
	//bool load_properties_from_file = true;
	//size_t object_file_size = 0;
	if ( !IsValid( pRequest->m_DataRequestId ) )
	{
		pRequest->m_DataRequestId = PipelineClient::GetStaticInstance()->BeginRequestAssetData( m_Metadata[ pRequest->m_Index ].m_Path );
		return false;
	}

	return false;

#if 0
		if (!object_file_path.IsFile())
		{
			if (pType->GetMetaClass()->IsType( Reflect::GetMetaClass< Resource >() ))
			{
				HELIUM_TRACE(
					TraceLevels::Info,
					TXT( "PipelinePackageLoader::TickDeserialize(): No object file found for resource \"%s\". Expected file location: \"%s\". This is normal for newly added resources.\n" ),
					*rObjectData.m_Path.ToString(),
					*object_file_path);

				// We will allow continuing to load using all default properties. This behavior is to support dropping resources into the 
				// data property and autogenerating objects from them.
				load_properties_from_file = false;
			}
			else
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "PipelinePackageLoader::TickDeserialize(): No object file found for object \"%s\". Expected file location: \"%s\"\n" ),
					*rObjectData.m_Path.ToString(),
					*object_file_path);
			}
		}
		else
		{
			Status status;
			status.Read( object_file_path.Get().c_str() );
			int64_t i64_object_file_size = status.m_Size;

			if( i64_object_file_size == -1 )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "PipelinePackageLoader::TickDeserialize(): Could not get file size for object file of object \"%s\". Expected file location: \"%s\"\n" ),
					*rObjectData.m_Path.ToString(),
					*object_file_path );
			}
			else if( i64_object_file_size == 0 )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "PipelinePackageLoader::TickDeserialize(): Object file \"%s\" for objct \"%s\" is empty.\n" ),
					*object_file_path,
					*rObjectData.m_Path.ToString() );
			}
			else if( static_cast< uint64_t >( i64_object_file_size ) > static_cast< uint64_t >( ~static_cast< size_t >( 0 ) ) )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "PipelinePackageLoader::TickDeserialize(): Object file \"%s\" exceeds the maximum size supported by " )
					TXT( "the current platform (file size: %" ) PRIu64 TXT( " bytes; max supported: %" ) PRIuSZ
					TXT( " bytes).\n" ) ),
					object_file_path.c_str(),
					static_cast< uint64_t >( i64_object_file_size ),
					~static_cast< size_t >( 0 ) );
			}
			else
			{
				object_file_size = static_cast< size_t >(i64_object_file_size);
			}
		}

		if (!load_properties_from_file)
		{
			HELIUM_ASSERT(!object_file_size);
		}
		else if (!object_file_size)
		{
			pRequest->flags |= LOAD_FLAG_PRELOADED | LOAD_FLAG_ERROR;
			return true;
		}
		else
		{
			HELIUM_ASSERT( !pRequest->pAsyncFileLoadBuffer );
			pRequest->pAsyncFileLoadBuffer = DefaultAllocator().Allocate( object_file_size );
			HELIUM_ASSERT( pRequest->pAsyncFileLoadBuffer );

			pRequest->asyncFileLoadBufferSize = object_file_size;

			pRequest->asyncFileLoadId = rAsyncLoader.QueueRequest(
				pRequest->pAsyncFileLoadBuffer,
				String(object_file_path.c_str()),
				0,
				pRequest->asyncFileLoadBufferSize);
		}

	}

	size_t bytesRead = 0;
	if (load_properties_from_file)
	{
		HELIUM_ASSERT( IsValid( pRequest->asyncFileLoadId ) );

		if ( !rAsyncLoader.TrySyncRequest( pRequest->asyncFileLoadId, bytesRead ) )
		{
			return false;
		}
	}

	/////// POINT OF NO RETURN: We *will* return true after this point, and the object *will* be finished preloading,
	/////// for good or for bad.

	SetInvalid(pRequest->asyncFileLoadId);
	bool object_creation_failure = false;

	// If we already had an existing object, make sure the type and template match.
	if( pAsset )
	{
		const AssetType* pExistingType = pAsset->GetAssetType();
		HELIUM_ASSERT( pExistingType );
		if( pExistingType != pType )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "PipelinePackageLoader: Cannot load \"%s\" using the existing object as the types do not match " )
				TXT( "(existing type: \"%s\"; serialized type: \"%s\".\n" ) ),
				*rObjectData.m_Path.ToString(),
				*pExistingType->GetName(),
				*pType->GetName() );

			pAsset->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
			pAsset->ConditionalFinalizeLoad();

			object_creation_failure = true;
		}
	}
	else
	{
		bool bCreateResult = false;
		if (pRequest->forceReload)
		{
			// Create the object.
			bCreateResult = Asset::CreateObject(
				pRequest->m_Asset,
				pType,
				Name( NULL_NAME ),
				NULL,
				pTemplate );
		}
		else
		{
			// Create the object.
			bCreateResult = Asset::CreateObject(
				pRequest->m_Asset,
				pType,
				rObjectData.m_Path.GetName(),
				pOwner,
				pTemplate );
		}

		if( !bCreateResult )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "PipelinePackageLoader: Failed to create \"%s\" during loading.\n" ),
				*rObjectData.m_Path.ToString() );

			object_creation_failure = true;
		}

		pAsset = pRequest->m_Asset;
		HELIUM_ASSERT( pAsset );
	}

	if (load_properties_from_file && !object_creation_failure)
	{
		// Sanity checks for file load, then success path
		HELIUM_ASSERT( bytesRead == pRequest->asyncFileLoadBufferSize );
		if( IsInvalid( bytesRead ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "PipelinePackageLoader: Failed to read the contents of object file \"%s\" in async load request \"%d\".\n" ),
				object_file_path.c_str(),
				pRequest->asyncFileLoadId );
		}
		else if( bytesRead != pRequest->asyncFileLoadBufferSize )
		{
			HELIUM_TRACE(
				TraceLevels::Warning,
				( TXT( "PipelinePackageLoader: Attempted to read %" ) PRIuSZ TXT( " bytes from object file \"%s\", " )
				TXT( "but only %" ) PRIuSZ TXT( " bytes were read.\n" ) ),
				pRequest->asyncFileLoadBufferSize,
				object_file_path.c_str(),
				bytesRead );
		}
		else
		{
			StaticMemoryStream archiveStream ( pRequest->pAsyncFileLoadBuffer, pRequest->asyncFileLoadBufferSize );

			HELIUM_TRACE(
				TraceLevels::Info,
				TXT( "PipelinePackageLoader: Reading %s. pResolver = %x\n"), 
				object_file_path.c_str(),
				pRequest->pResolver);

			DynamicArray< Reflect::ObjectPtr > objects;
			objects.Push( pRequest->m_Asset.Get() ); // use existing objects
			Persist::ArchiveReaderJson::ReadFromStream( archiveStream, objects, pRequest->pResolver );
			HELIUM_ASSERT( objects[0].Get() == pRequest->spAsset.Get() );
		}
	}

	if (load_properties_from_file)
	{
		DefaultAllocator().Free(pRequest->pAsyncFileLoadBuffer);
		pRequest->pAsyncFileLoadBuffer = NULL;
		pRequest->asyncFileLoadBufferSize = 0;
	}

	pRequest->flags |= LOAD_FLAG_PROPERTY_PRELOADED;

	if( object_creation_failure )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "PipelinePackageLoader: Deserialization of object \"%s\" failed.\n" ),
			*rObjectData.m_Path.ToString() );

		pAsset->SetFlags( Asset::FLAG_PRELOADED | Asset::FLAG_LINKED );
		pAsset->ConditionalFinalizeLoad();

		pRequest->flags |= LOAD_FLAG_ERROR;
	}
	else if( !pAsset->IsDefaultTemplate() )
	{
		// If the object is a resource (not including the default template object for resource types), attempt to begin
		// loading any existing persistent resource data stored in the object cache.
		Resource* pResource = Reflect::SafeCast< Resource >( pAsset );
		if( pResource )
		{
			Name objectCacheName = Name( HELIUM_ASSET_CACHE_NAME );
			CacheManager& rCacheManager = CacheManager::GetStaticInstance();

			Cache* pCache = rCacheManager.GetCache( objectCacheName );
			HELIUM_ASSERT( pCache );
			pCache->EnforceTocLoad();

			const Cache::Entry* pEntry = pCache->FindEntry( rObjectData.m_Path, 0 );
			if( pEntry && pEntry->size != 0 )
			{
				HELIUM_ASSERT( IsInvalid( pRequest->persistentResourceDataLoadId ) );
				HELIUM_ASSERT( !pRequest->pCachedObjectDataBuffer );

				pRequest->pCachedObjectDataBuffer =
					static_cast< uint8_t* >( DefaultAllocator().Allocate( pEntry->size ) );
				HELIUM_ASSERT( pRequest->pCachedObjectDataBuffer );
				pRequest->cachedObjectDataBufferSize = pEntry->size;

				AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();
				pRequest->persistentResourceDataLoadId = rAsyncLoader.QueueRequest(
					pRequest->pCachedObjectDataBuffer,
					pCache->GetCacheFileName(),
					pEntry->offset,
					pEntry->size );
				HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
			}
		}
	}

	if( IsInvalid( pRequest->persistentResourceDataLoadId ) )
	{
		// No persistent resource data needs to be loaded.
		pAsset->SetFlags( Asset::FLAG_PRELOADED );
		pRequest->flags |= LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED;
	}

	// Asset is now preloaded.
	return true;

#endif
}

#if 0
/// Update processing of persistent resource data loading for a given load request.
///
/// @param[in] pRequest  Load request to process.
///
/// @return  True if persistent resource data loading for the given load request has completed, false if not.
bool PipelinePackageLoader::TickPersistentResourcePreload( LoadRequest* pRequest )
{
	HELIUM_ASSERT( pRequest );
	HELIUM_ASSERT( !( pRequest->flags & LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED ) );

	Resource* pResource = Reflect::AssertCast< Resource >( pRequest->m_Asset.Get() );
	HELIUM_ASSERT( pResource );

	// Wait for the cached data load to complete.
	AsyncLoader& rAsyncLoader = AsyncLoader::GetStaticInstance();

	size_t bytesRead = 0;
	HELIUM_ASSERT( IsValid( pRequest->persistentResourceDataLoadId ) );
	if( !rAsyncLoader.TrySyncRequest( pRequest->persistentResourceDataLoadId, bytesRead ) )
	{
		return false;
	}

	SetInvalid( pRequest->persistentResourceDataLoadId );

	if( bytesRead != pRequest->cachedObjectDataBufferSize )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			( TXT( "PipelinePackageLoader: Requested load of %" ) PRIu32 TXT( " bytes from cached object data for " )
			TXT( "\"%s\", but only %" ) PRIuSZ TXT( " bytes were read.\n" ) ),
			pRequest->cachedObjectDataBufferSize,
			*pResource->GetPath().ToString(),
			bytesRead );
	}

	// Make sure we read enough bytes to cover the object property data size.
	if( bytesRead < sizeof( uint32_t ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			( TXT( "PipelinePackageLoader: Not enough bytes read of cached object data \"%s\" from which to parse the " )
			TXT( "property stream size.\n" ) ),
			*pResource->GetPath().ToString() );

		pRequest->flags |= LOAD_FLAG_ERROR;
	}
	else
	{
		// Skip over the object property data.
		uint8_t* pCachedObjectData = pRequest->pCachedObjectDataBuffer;
		HELIUM_ASSERT( pCachedObjectData );

		uint32_t propertyDataSize = *reinterpret_cast< uint32_t* >( pCachedObjectData );

		size_t byteSkipCount = sizeof( propertyDataSize ) + propertyDataSize;
		if( byteSkipCount > bytesRead )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "PipelinePackageLoader: Cached persistent resource data for \"%s\" extends past the end of the " )
				TXT( "cached data stream.\n" ) ),
				*pResource->GetPath().ToString() );

			pRequest->flags |= LOAD_FLAG_ERROR;
		}
		else
		{
			pCachedObjectData += byteSkipCount;
			size_t bytesRemaining = bytesRead - byteSkipCount;

			// Make sure we have enough bytes at the end for the resource sub-data count.
			if( bytesRemaining < sizeof( uint32_t ) )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "PipelinePackageLoader: Not enough space is reserved in the cached persistent resource " )
					TXT( "data stream for \"%s\" for the resource sub-data count.\n" ) ),
					*pResource->GetPath().ToString() );

				pRequest->flags |= LOAD_FLAG_ERROR;
			}
			else
			{
				bytesRemaining -= sizeof( uint32_t );

				//// Deserialize the persistent resource data.
				//// Having to do this copy is unfortunate.. maybe we can revisit this later
				//std::stringstream ss_in;
				//ss_in.write(reinterpret_cast<char *>(pCachedObjectData), bytesRemaining);

				//Reflect::ArchiveBinary archive(new Reflect::CharStream(&ss_in, false, Helium::ByteOrders::LittleEndian, Helium::Reflect::CharacterEncodings::UTF_16), false);

				//Reflect::ObjectPtr persistent_data;
				//archive.ReadSingleObject(persistent_data);

				Reflect::ObjectPtr persistent_data;
				persistent_data = Cache::ReadCacheObjectFromBuffer(pCachedObjectData, /*sizeof( uint32_t )*/ 0, bytesRemaining, pRequest->pResolver);

				if (!pResource->LoadPersistentResourceObject(persistent_data))
				{
					HELIUM_TRACE(
						TraceLevels::Error,
						( TXT( "PipelinePackageLoader: Failed to load persistent resource object for \"%s\".\n" ) ),
						*pResource->GetPath().ToString() );

					pRequest->flags |= LOAD_FLAG_ERROR;
				}
			}
		}
	}

	DefaultAllocator().Free( pRequest->pCachedObjectDataBuffer );
	pRequest->pCachedObjectDataBuffer = NULL;
	pRequest->cachedObjectDataBufferSize = 0;

	pResource->SetFlags( Asset::FLAG_PRELOADED );

	pRequest->flags |= LOAD_FLAG_PERSISTENT_RESOURCE_PRELOADED;

	return true;
}
#endif