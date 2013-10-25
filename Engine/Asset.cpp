#include "EnginePch.h"
#include "Engine/Asset.h"

#include "Foundation/ObjectPool.h"
#include "Engine/Asset.h"
#include "Engine/PackageLoader.h"

HELIUM_DEFINE_CLASS_NO_REGISTRAR( Helium::Asset )

using namespace Helium;

SparseArray< AssetWPtr > Asset::sm_objects;
AssetWPtr Asset::sm_wpFirstTopLevelObject;

Asset::ChildNameInstanceIndexMap* Asset::sm_pNameInstanceIndexMap = NULL;
Pair< AssetPath, Asset::NameInstanceIndexMap >* Asset::sm_pEmptyNameInstanceIndexMap = NULL;
Pair< Name, Asset::InstanceIndexSet >* Asset::sm_pEmptyInstanceIndexSet = NULL;

ReadWriteLock Asset::sm_objectListLock;

DynamicArray< uint8_t > Asset::sm_serializationBuffer;

/// Constructor.
Asset::Asset()
	: m_name( NULL_NAME )
	, m_instanceIndex( Invalid< uint32_t >() )
	, m_id( Invalid< uint32_t >() )
	, m_flags( 0 )
	, m_path( NULL_NAME )
	, m_pCustomDestroyCallback( NULL )
{
}

/// Destructor.
Asset::~Asset()
{
	HELIUM_ASSERT_MSG(
		GetAnyFlagSet( Asset::FLAG_PREDESTROYED ),
		TXT( "Asset::PreDestroy() not called prior to destruction." ) );
}

void Asset::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(            &Asset::m_spTemplate,               TXT( "m_Template" ) , Reflect::FieldFlags::Hide);
}


/// Modify the name, owner, or instance index of this object.
///
/// @param[in] rParameters  Object rename parameters.
///
/// @return  True if this object was renamed successfully, false if not.
///
/// @see GetName(), GetOwner(), GetInstanceIndex()
bool Asset::Rename( const RenameParameters& rParameters )
{
	Name name = rParameters.name;
	Asset* pOwner = rParameters.spOwner;
	uint32_t instanceIndex = rParameters.instanceIndex;

	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT("Asset::Rename(): Renaming object \"%s\" to \"%s\" (Old Owner: \"%s\". New Owner: \"%s\".)\n"),
		*m_name,
		*rParameters.name,
		m_spOwner.ReferencesObject() ? *m_spOwner->GetPath().ToString() : TXT("[none]"),
		rParameters.spOwner.ReferencesObject() ? *rParameters.spOwner->GetPath().ToString() : TXT("[none]"));

	// Only allow setting an empty name if no owner or instance index are given and this object has no children.
	if( name.IsEmpty() )
	{
		HELIUM_ASSERT( !pOwner );
		HELIUM_ASSERT( IsInvalid( instanceIndex ) );
		if( pOwner || IsValid( instanceIndex ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "Asset::Rename(): Objects cannot have name information cleared if being assigned an " )
				  TXT( "owner or instance index.\n" ) ) );

			return false;
		}

		HELIUM_ASSERT( !m_wpFirstChild );
		if( m_wpFirstChild )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "Asset::Rename(): Cannot clear name information for objects with children.\n" ) );

			return false;
		}
	}

	// Don't allow setting the owner to ourself.
	if( pOwner == this )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Asset::Rename(): Cannot set the owner of an object to itself.\n" ) );

		return false;
	}

	// Don't allow setting the owner to an object with no name information.
	if( pOwner && pOwner->m_name.IsEmpty() )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Asset::Rename(): Cannot set the owner of an object to an object with no path information.\n" ) );

		return false;
	}

	if( IsPackage() )
	{
		// Don't allow package objects to be children of non-package objects.
		if( pOwner && !pOwner->IsPackage() )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "Asset::Rename(): Cannot set a non-package as the owner of a package.\n" ) );

			return false;
		}

		// Don't allow instance indexing for packages.
		if( IsValid( instanceIndex ) )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "Asset::Rename(): Instance indexing not supported for packages.\n" ) );

			return false;
		}
	}

	// Don't need to do anything if the name, owner, and instance index are not changing.
	if( name == m_name &&
		pOwner == m_spOwner &&
		( instanceIndex == m_instanceIndex || ( instanceIndex == INSTANCE_INDEX_AUTO && IsValid( m_instanceIndex ) ) ) )
	{
		return true;
	}

	// Hold onto a reference to the current owner until we return from this function.  This is done in case this object
	// has the last strong reference to it, in which case we would encounter a deadlock if clearing its reference while
	// we still have a write lock on the object list (object destruction also requires acquiring a write lock).
	AssetPtr spOldOwner = m_spOwner;

	{
		// Acquire a write lock on the object list to prevent objects from being added and removed as well as keep
		// objects from being renamed while this object is being renamed.
		ScopeWriteLock scopeLock( sm_objectListLock );

		// Get the list of children belonging to the new owner.
		AssetWPtr& rwpOwnerFirstChild = ( pOwner ? pOwner->m_wpFirstChild : sm_wpFirstTopLevelObject );

		// Don't check for name clashes if we're clearing the object path name information.
		if( !name.IsEmpty() )
		{
			// Resolve name clashes either through the instance index lookup map (if an instance index will be assigned)
			// or through a child object search (if no instance index will be used).
			if( IsValid( instanceIndex ) )
			{
				// Get the instance index map for the requested object name.
				ChildNameInstanceIndexMap& rNameInstanceIndexMap = GetNameInstanceIndexMap();
				HELIUM_ASSERT( sm_pEmptyNameInstanceIndexMap );
				HELIUM_ASSERT( sm_pEmptyInstanceIndexSet );

				sm_pEmptyNameInstanceIndexMap->First() = ( pOwner ? pOwner->GetPath() : AssetPath( NULL_NAME ) );
				sm_pEmptyInstanceIndexSet->First() = name;

				ChildNameInstanceIndexMap::Accessor childNameMapAccessor;
				rNameInstanceIndexMap.Insert( childNameMapAccessor, *sm_pEmptyNameInstanceIndexMap );

				NameInstanceIndexMap::Accessor indexSetAccessor;
				childNameMapAccessor->Second().Insert( indexSetAccessor, *sm_pEmptyInstanceIndexSet );

				InstanceIndexSet& rIndexSet = indexSetAccessor->Second();
				InstanceIndexSet::ConstAccessor indexAccessor;

				if( instanceIndex == INSTANCE_INDEX_AUTO )
				{
					// Pick an unused instance index.
					instanceIndex = 0;
					while( !rIndexSet.Insert( indexAccessor, instanceIndex ) )
					{
						++instanceIndex;
						HELIUM_ASSERT( instanceIndex < INSTANCE_INDEX_AUTO );
					}
				}
				else
				{
					// Attempt to acquire the specified instance index.
					if( !rIndexSet.Insert( indexAccessor, instanceIndex ) )
					{
						HELIUM_TRACE(
							TraceLevels::Error,
							( TXT( "Asset::Rename(): Object already exists with the specified owner (%s), name " )
							  TXT( "(%s), and instance index (%" ) PRIu32 ").\n" ),
							( pOwner ? *pOwner->GetPath().ToString() : TXT( "none" ) ),
							*name,
							instanceIndex );

						return false;
					}
				}
			}
			else
			{
				// Check each child of the new owner for a name clash.
				for( Asset* pChild = rwpOwnerFirstChild; pChild != NULL; pChild = pChild->m_wpNextSibling )
				{
					if( pChild->m_name == name && pChild->m_instanceIndex == instanceIndex )
					{
						HELIUM_TRACE(
							TraceLevels::Error,
							( TXT( "Asset::Rename(): Object already exists with the specified owner (%s) and " )
							  TXT( "name (%s).\n" ) ),
							( pOwner ? *pOwner->GetPath().ToString() : TXT( "none" ) ),
							*name );

						return false;
					}
				}
			}
		}

		// Remove any old instance index tracking for the old path name.
		if( IsValid( m_instanceIndex ) )
		{
			AssetPath ownerPath = ( spOldOwner ? spOldOwner->GetPath() : AssetPath( NULL_NAME ) );

			ChildNameInstanceIndexMap& rNameInstanceIndexMap = GetNameInstanceIndexMap();

			ChildNameInstanceIndexMap::Accessor childMapAccessor;
			HELIUM_VERIFY( rNameInstanceIndexMap.Find( childMapAccessor, ownerPath ) );

			NameInstanceIndexMap& rNameMap = childMapAccessor->Second();
			NameInstanceIndexMap::Accessor nameMapAccessor;
			HELIUM_VERIFY( rNameMap.Find( nameMapAccessor, m_name ) );

			InstanceIndexSet& rIndexSet = nameMapAccessor->Second();
			HELIUM_VERIFY( rIndexSet.Remove( m_instanceIndex ) );

			/*
			if( rIndexSet.IsEmpty() )
			{
				HELIUM_VERIFY( rNameMap.Remove( nameMapAccessor ) );
				if( rNameMap.IsEmpty() )
				{
					HELIUM_VERIFY( rNameInstanceIndexMap.Remove( childMapAccessor ) );
				}
			}
			*/
		}

		// If the owner of this object is changing, remove this object from its old owner's list and add it to the new
		// owner.
		if( spOldOwner.Get() != pOwner || ( m_name.IsEmpty() ? !name.IsEmpty() : name.IsEmpty() ) )
		{
			// Object should not be in any child object lists if its name is empty.
			if( !m_name.IsEmpty() )
			{
				AssetWPtr& rwpOldOwnerFirstChild =
					( spOldOwner ? spOldOwner->m_wpFirstChild : sm_wpFirstTopLevelObject );

				Asset* pPreviousChild = NULL;
				Asset* pChild = rwpOldOwnerFirstChild;
				while( pChild )
				{
					if( pChild == this )
					{
						( pPreviousChild ? pPreviousChild->m_wpNextSibling : rwpOldOwnerFirstChild ) = m_wpNextSibling;
						m_wpNextSibling.Release();

						break;
					}

					pPreviousChild = pChild;
					pChild = pChild->m_wpNextSibling;
				}
			}

			// If you assert here, it's possible that sm_wpFirstTopLevelObject is null. This can happen if an asset is destroyed
			// after the asset system is shut down
			HELIUM_ASSERT( !m_wpNextSibling );

			// Only store the object in a child object list if it is being given a valid name.
			if( !name.IsEmpty() )
			{
				m_wpNextSibling = rwpOwnerFirstChild;
				rwpOwnerFirstChild = this;
			}
		}

		// Set the new path name.
		m_name = name;
		m_spOwner = pOwner;
		m_instanceIndex = instanceIndex;

		// Update path information for this object and its children.
		UpdatePath();
	}

	return true;
}

Helium::Reflect::ObjectPtr Helium::Asset::Clone()
{
	AssetPtr gop;
	CreateObject(gop, GetAssetType(), m_name, m_spOwner.Get(), this, true);
	return gop.Ptr();
}

bool Helium::Asset::CloneAsset(AssetPtr _asset_ptr)
{
	return CreateObject(_asset_ptr, GetAssetType(), m_name, m_spOwner.Get(), this, true);
}

/// Set all object flags covered by the given mask.
///
/// Note that all object flag functions are thread-safe.
///
/// @param[in] flagMask  Asset flag bit mask.
///
/// @return  Asset flag state immediately prior to setting the given flags.
///
/// @see ClearFlags(), ToggleFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
uint32_t Asset::SetFlags( uint32_t flagMask )
{
	HELIUM_ASSERT( flagMask != 0 );

	return AtomicOr( reinterpret_cast< volatile int32_t& >( m_flags ), flagMask );
}

/// Clear all object flags covered by the given mask.
///
/// Note that all object flag functions are thread-safe.
///
/// @param[in] flagMask  Asset flag bit mask.
///
/// @return  Asset flag state immediately prior to clearing the given flags.
///
/// @see SetFlags(), ToggleFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
uint32_t Asset::ClearFlags( uint32_t flagMask )
{
	HELIUM_ASSERT( flagMask != 0 );

	return AtomicAnd( reinterpret_cast< volatile int32_t& >( m_flags ), ~flagMask );
}

/// Toggle all object flags covered by the given mask.
///
/// Note that all object flag functions are thread-safe.
///
/// @param[in] flagMask  Asset flag bit mask.
///
/// @return  Asset flag state immediately prior to clearing the given flags.
///
/// @see SetFlags(), ClearFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
uint32_t Asset::ToggleFlags( uint32_t flagMask )
{
	HELIUM_ASSERT( flagMask != 0 );

	return AtomicXor( reinterpret_cast< volatile int32_t& >( m_flags ), flagMask );
}

/// Get the template for this object.
///
/// @return  Object template.
Reflect::ObjectPtr Asset::GetTemplate() const
{
	return Reflect::ObjectPtr( GetTemplateAsset().Get() );
}

AssetPtr Asset::GetTemplateAsset() const
{
	Asset* pTemplate = m_spTemplate;
	if( !pTemplate )
	{
		const AssetType* pType = GetAssetType();
		HELIUM_ASSERT( pType );
		pTemplate = pType->GetTemplate();
		HELIUM_ASSERT( pTemplate );
	}

	return pTemplate;
}


/// Search for a direct child of this object with the given name.
///
/// @param[in] name           Asset name.
/// @param[in] instanceIndex  Asset instance index.
///
/// @return  Pointer to the child object if found, null if not found.
Asset* Asset::FindChild( Name name, uint32_t instanceIndex ) const
{
	return FindChildOf( this, name, instanceIndex );
}

/// @copydoc Object::PreDestroy()
void Asset::RefCountPreDestroy()
{
	HELIUM_VERIFY( Rename( RenameParameters() ) );

	if( IsValid( m_id ) )
	{
		UnregisterObject( this );
	}

	SetFlags( Asset::FLAG_PREDESTROYED );
}

/// @copydoc Object::RefCountDestroy()
void Asset::RefCountDestroy()
{
	HELIUM_ASSERT( !GetRefCountProxy() || GetRefCountProxy()->GetStrongRefCount() == 0 );

	if( m_pCustomDestroyCallback )
	{
		m_pCustomDestroyCallback( this );
	}
	else
	{
		delete this;
	}
}

/// Get the type of this object.
///
/// @return  Asset type.
const AssetType* Asset::GetAssetType() const
{
	return Asset::GetStaticType();
}

/// Get whether this object requires precaching of resource data during loading.
///
/// @return  True if precaching is necessary, false if not.
///
/// @see BeginPrecacheResourceData()
bool Asset::NeedsPrecacheResourceData() const
{
	return false;
}

/// Begin precaching of resource data for this object during loading.
///
/// Precaching begins after an object is linked and all of its dependencies are fully loaded, so it is safe to use
/// referenced objects when this is called.
///
/// @return  True if precaching was started successfully, false if not.
///
/// @see TryFinishPrecacheResourceData(), NeedsPrecacheResourceData()
bool Asset::BeginPrecacheResourceData()
{
	return false;
}

/// Attempt to finalize resource precaching for this object without blocking.
///
/// Precaching begins after an object is linked and all of its dependencies are fully loaded, so it is safe to use
/// referenced objects when this is called.
///
/// @return  True if precaching was completed or is not in progress, false if it still requires time to process.
///
/// @see BeginPrecacheResourceData(), NeedsPrecacheResourceData()
bool Asset::TryFinishPrecacheResourceData()
{
	return true;
}

/// Perform any work once the load process for an object and its dependencies has completed.
///
/// This is called once an object has been loaded, linked, and all potentially dependent objects have been loaded
/// and linked as well.
void Asset::FinalizeLoad()
{
}

#if HELIUM_TOOLS
/// Perform any work immediately after saving/caching an object in the editor.
void Asset::PostSave()
{
}


Asset *Helium::Asset::GetSourceAsset()
{
	HELIUM_ASSERT( !m_path.IsEmpty() );

	Asset *pAssetWithSourceFile = this;
	while ( pAssetWithSourceFile && pAssetWithSourceFile->GetAssetType()->GetFlags() & AssetType::FLAG_GENERATED_FROM_OWNER )
	{
		pAssetWithSourceFile = pAssetWithSourceFile->GetOwner();
	}

	return pAssetWithSourceFile;
}

const FilePath *Helium::Asset::GetAssetFileSystemPath()
{
	HELIUM_ASSERT( !m_path.IsEmpty() );

	FilePath filePath;

	Asset *pSourceAsset = GetSourceAsset();
	if (pSourceAsset)
	{
		Package *pPackage = Reflect::SafeCast<Package>( pSourceAsset->GetOwner() );

		if ( pPackage )
		{
			PackageLoader *pLoader = pPackage->GetLoader();
			HELIUM_ASSERT( pLoader->HasAssetFileState() );

			if ( pLoader )
			{
				return &pLoader->GetAssetFileSystemPath( pSourceAsset->GetPath() );
			}
		}
	}

	return NULL;
}

uint64_t Helium::Asset::GetAssetFileTimeStamp()
{
	HELIUM_ASSERT( !m_path.IsEmpty() );
	uint64_t timestamp = 0;

	Asset *pSourceAsset = GetSourceAsset();
	if (pSourceAsset)
	{
		Package *pPackage = Reflect::SafeCast<Package>( pSourceAsset->GetOwner() );

		if ( pPackage )
		{
			PackageLoader *pLoader = pPackage->GetLoader();
			HELIUM_ASSERT( pLoader->HasAssetFileState() );

			if ( pLoader )
			{
				pLoader->GetAssetFileSystemTimestamp( pSourceAsset->GetPath() );
			}
		}
	}

	return timestamp;
}

struct AssetFixup
{
	Asset *pAsset;
};

void Asset::ReplaceObject( Asset* pNewAsset, const AssetPath &objectToReplace )
{
	// Acquire a write lock on the object list to prevent objects from being added and removed as well as keep
	// objects from being renamed while this object is being renamed.
	ScopeWriteLock scopeLock( sm_objectListLock );

	Asset *pOldAsset = Asset::FindObject( objectToReplace );
	HELIUM_ASSERT( pNewAsset->GetMetaClass()->IsType( pOldAsset->GetMetaClass() ) );

	DynamicArray<AssetFixup> fixups;

	// For every asset in existence
	for ( SparseArray< AssetWPtr >::Iterator iter = sm_objects.Begin();
		iter != sm_objects.End(); ++iter)
	{
		if ( !iter )
		{
			continue;
		}

		Asset* pPossibleFixupAsset = iter->Get();
		if ( !pPossibleFixupAsset || pPossibleFixupAsset->IsDefaultTemplate() )
		{
			continue;
		}

		// Ignore it if it's the asset we're swapping
		if ( pPossibleFixupAsset->m_id == pNewAsset->m_id || pPossibleFixupAsset->m_id == pOldAsset->m_id )
		{
			continue;
		}
		
		// If the asset has the old asset as a template (direct or indirect)
		Asset *pTemplate = pPossibleFixupAsset->GetTemplateAsset().Get();
		
		while ( pTemplate && !pTemplate->IsDefaultTemplate() )
		{
			if ( pTemplate == pOldAsset )
			{
				// Then save it off
				AssetFixup &fixup = *fixups.New();
				fixup.pAsset = pPossibleFixupAsset;
				break;
			}

			pTemplate = pTemplate->GetTemplateAsset().Get();
		}
	}

	const Reflect::MetaStruct *pStruct = pOldAsset->GetMetaClass();

	// Get all the bases
#pragma TODO("Declare a max depth for inheritance to save heap allocs -geoff")
	DynamicArray< const Reflect::MetaStruct* > bases;
	for ( const Reflect::MetaStruct* current = pStruct; current != NULL; current = current->m_Base )
	{
		bases.Push( current );
	}

	// For all assets that depend on the changing asset
	for ( DynamicArray<AssetFixup>::Iterator iter = fixups.Begin();
		iter != fixups.End(); ++iter)
	{
		// Get the template (old) asset's type
		Asset *pAsset = iter->pAsset;

		// Get all the fields that should be modified due to the base template changing
#pragma TODO("Declare a max count for fields to save heap allocs -geoff")
		DynamicArray< const Reflect::Field* > fields;
		while ( !bases.IsEmpty() )
		{
			const Reflect::MetaStruct* current = bases.Pop();
			DynamicArray< Reflect::Field >::ConstIterator itr = current->m_Fields.Begin();
			DynamicArray< Reflect::Field >::ConstIterator end = current->m_Fields.End();
			for ( ; itr != end; ++itr )
			{
				// Field is guaranteed to be in all objects of the template dependency chain
				const Reflect::Field* field = &*itr;

				// For every field
				for ( uint32_t i = 0; i < field->m_Count; ++i )
				{
					bool isOverridden = false;
					Reflect::Pointer assetPointer( field, pAsset, i );

					// Is there anywhere in the template chain that the field gets overridden?
					Asset *pTemplate = pAsset->GetTemplateAsset();
					while ( pTemplate )
					{
						Reflect::Pointer templatePointer( field, pTemplate, i );

						if ( !field->m_Translator->Equals( assetPointer, templatePointer ) )
						{
							// Yes, bail
							isOverridden = true;
							break;
						}

						pTemplate == pOldAsset ? pTemplate = NULL : pTemplate->GetTemplate();
					}

					if ( !isOverridden )
					{
						Reflect::Pointer newAssetPointer( field, pNewAsset, i );
						field->m_Translator->Copy( newAssetPointer, assetPointer, Reflect::CopyFlags::Shallow );
					}
				}
			}
		}
	}

	pNewAsset->RefCountSwapProxies( pOldAsset );
	Helium::Swap( pOldAsset->m_name, pNewAsset->m_name );
	Helium::Swap( pOldAsset->m_instanceIndex, pNewAsset->m_instanceIndex );
	Helium::Swap( pOldAsset->m_path, pNewAsset->m_path );
	Helium::Swap( pOldAsset->m_id, pNewAsset->m_id );
	Helium::Swap( pOldAsset->m_spOwner, pNewAsset->m_spOwner );
	Helium::Swap( pOldAsset->m_wpFirstChild, pNewAsset->m_wpFirstChild );
	Helium::Swap( pOldAsset->m_wpNextSibling, pNewAsset->m_wpNextSibling );
}

#endif  // HELIUM_TOOLS

/// Get whether this object is transient.
///
/// Transient objects are not saved into or loaded from a package stored on disk.  An object is transient if its
/// type or the types of any of its owners have the AssetType::FLAG_TRANSIENT flag set, or if it or one of its
/// parents have the Asset::FLAG_TRANSIENT flag set.
///
/// @return  True if this object is transient, false if not.
bool Asset::IsTransient() const
{
	for( const Asset* pObject = this; pObject != NULL && !pObject->IsPackage(); pObject = pObject->GetOwner() )
	{
		if( pObject->GetAnyFlagSet( Asset::FLAG_TRANSIENT ) )
		{
			return true;
		}

		const AssetType* pType = pObject->GetAssetType();
		HELIUM_ASSERT( pType );
		if( pType->GetFlags() & AssetType::FLAG_TRANSIENT )
		{
			return true;
		}
	}

	return false;
}

/// Get the size of an instance of this object (used for in-place object construction).
///
/// @return  Size of an instance of this object, in bytes.
///
/// @see InPlaceConstruct(), InPlaceDestroy()
size_t Asset::GetInstanceSize() const
{
	return sizeof( *this );
}

/// Construct an instance of this object in-place.
///
/// @param[in] pMemory           Memory buffer in which to construct an instance of this object.
/// @param[in] pDestroyCallback  Callback to use when notifying that this object should be destroyed (when its
///                              reference count reaches zero).  This must be specified.
///
/// @return  Pointer to the constructed object instance.
///
/// @see InPlaceDestroy(), GetInstanceSize()
Asset* Asset::InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const
{
	HELIUM_ASSERT( pMemory );
	HELIUM_ASSERT( pDestroyCallback );

	Asset* pObject = new( pMemory ) Asset;
	pObject->SetCustomDestroyCallback( pDestroyCallback );

	return pObject;
}

/// Destroy this object instance in-place.
///
/// This should only be called if this object was constructed in-place, either manually or from calling
/// InPlaceConstructor().
///
/// @see InPlaceConstruct(), GetInstanceSize()
void Asset::InPlaceDestroy()
{
	this->~Asset();
}

/// Create a new object.
///
/// @param[out] rspObject             Pointer to the newly created object if object creation was successful.  Note that
///                                   any object reference stored in this strong pointer prior to calling this function
///                                   will always be cleared by this function, regardless of whether object creation is
///                                   successful.
/// @param[in]  pType                 Type of object to create.
/// @param[in]  name                  Object name.
/// @param[in]  pOwner                Object owner.
/// @param[in]  pTemplate             Optional override template object.  If null, the default template for the
///                                   specified type will be used.
/// @param[in]  bAssignInstanceIndex  True to assign an instance index to the object, false to leave the index
///                                   invalid.
///
/// @return  True if object creation was successful, false if not.
///
/// @see Create()
bool Asset::CreateObject(
	AssetPtr& rspObject,
	const AssetType* pType,
	Name name,
	Asset* pOwner,
	Asset* pTemplate,
	bool bAssignInstanceIndex )
{
	HELIUM_ASSERT( pType );

	HELIUM_TRACE(
		TraceLevels::Debug,
		TXT( "Asset::CreateObject(): Creating object named \"%s\" of type \"%s\" owned by \"%s\".\n"),
		*name,
		*pType->GetName(),
		!pOwner ? TXT("[none]") : *pOwner->GetPath().ToString());

	rspObject.Release();

	// Get the appropriate template object.
	Asset* pObjectTemplate = pTemplate;
	if( pObjectTemplate )
	{
		if( pType->GetFlags() & AssetType::FLAG_NO_TEMPLATE && pType->GetTemplate() != pObjectTemplate )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				TXT( "Asset::CreateObject(): Objects of type \"%s\" cannot be used as templates.\n" ),
				*pType->GetName() );

			return false;
		}
	}
	else
	{
		pObjectTemplate = pType->GetTemplate();
		HELIUM_ASSERT( pObjectTemplate );
	}

	// Make sure the object template is of the correct type.
	if( !pObjectTemplate->IsInstanceOf( pType ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Asset::CreateObject: Template object \"%s\" is not of type \"%s\".\n" ),
			*pTemplate->GetPath().ToString(),
			pType->GetName().Get() );
		HELIUM_BREAK();

		return false;
	}

	// Allocate memory for and create the object.
	DefaultAllocator allocator;

	size_t bufferSize = pObjectTemplate->GetInstanceSize();
	void* pObjectMemory = allocator.AllocateAligned( HELIUM_SIMD_ALIGNMENT, bufferSize );
	HELIUM_ASSERT( pObjectMemory );
	Asset* pObject = pObjectTemplate->InPlaceConstruct( pObjectMemory, StandardCustomDestroy );
	HELIUM_ASSERT( pObject == pObjectMemory );
	rspObject = pObject;

	// Initialize the object based on its default.
	pObjectTemplate->CopyTo(pObject);
	
	pObject->m_spTemplate = pTemplate;

	// Attempt to register the object and set its name.
	RenameParameters nameParameters;
	nameParameters.name = name;
	nameParameters.spOwner = pOwner;
	if( bAssignInstanceIndex )
	{
		nameParameters.instanceIndex = INSTANCE_INDEX_AUTO;
	}

	if ( !RegisterObject( pObject ) )
	{            
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Asset::CreateObject(): RegisterObject() failed for Asset \"%s\" owned by \"%s\".\n" ),
			*name,
			!pOwner ? TXT("[none]") : *pOwner->GetPath().ToString());

		HELIUM_BREAK();

		rspObject.Release();

		return false;
	}

	if( !pObject->Rename( nameParameters ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "Asset::CreateObject(): Rename() failed for Asset \"%s\" owned by \"%s\".\n" ),
			*name,
			!pOwner ? TXT("[none]") : *pOwner->GetPath().ToString());

		HELIUM_BREAK();

		rspObject.Release();

		return false;
	}

	return true;
}

/// Find an object based on its path name.
///
/// @param[in] path  FilePath of the object to locate.
///
/// @return  Pointer to the object if found, null pointer if not found.
Asset* Asset::FindObject( AssetPath path )
{
	// Make sure the path isn't empty.
	if( path.IsEmpty() )
	{
		return NULL;
	}

	// Assemble a list of object names and instance indices, from the top level on down.
	size_t pathDepth = 0;
	size_t packageDepth = 0;
	for( AssetPath testPath = path; !testPath.IsEmpty(); testPath = testPath.GetParent() )
	{
		++pathDepth;

		if( testPath.IsPackage() )
		{
			++packageDepth;
		}
	}

	StackMemoryHeap<>& rStackHeap = ThreadLocalStackAllocator::GetMemoryHeap();
	StackMemoryHeap<>::Marker stackMarker( rStackHeap );

	Name* pPathNames = static_cast< Name* >( rStackHeap.Allocate( sizeof( Name ) * pathDepth ) );
	HELIUM_ASSERT( pPathNames );

	uint32_t* pInstanceIndices = static_cast< uint32_t* >( rStackHeap.Allocate( sizeof( uint32_t ) * pathDepth ) );
	HELIUM_ASSERT( pInstanceIndices );

	size_t pathIndex = pathDepth;
	for( AssetPath testPath = path; !testPath.IsEmpty(); testPath = testPath.GetParent() )
	{
		HELIUM_ASSERT( pathIndex != 0 );
		--pathIndex;

		pPathNames[ pathIndex ] = testPath.GetName();
		pInstanceIndices[ pathIndex ] = testPath.GetInstanceIndex();
	}

	HELIUM_ASSERT( pathIndex == 0 );

	// Search from the root.
	return FindChildOf( NULL, pPathNames, pInstanceIndices, pathDepth, packageDepth );
}

/// Search for a direct child of the specified object with the given name.
///
/// @param[in] pObject        Asset for which to locate a child, or null to search through top-level objects.
/// @param[in] name           Asset name.
/// @param[in] instanceIndex  Asset instance index.
///
/// @return  Pointer to the child object if found, null if not found.
Asset* Asset::FindChildOf( const Asset* pObject, Name name, uint32_t instanceIndex )
{
	HELIUM_ASSERT( !name.IsEmpty() );
	if( name.IsEmpty() )
	{
		return NULL;
	}

	ScopeReadLock scopeLock( sm_objectListLock );

	for( Asset* pChild = ( pObject ? pObject->m_wpFirstChild : sm_wpFirstTopLevelObject );
		 pChild != NULL;
		 pChild = pChild->m_wpNextSibling )
	{
		if( pChild->GetName() == name && pChild->GetInstanceIndex() == instanceIndex )
		{
			return pChild;
		}
	}

	return NULL;
}

/// Search for a child or grandchild of the given object with a relative path dictated by the given parameters.
///
/// @param[in] pObject             Asset for which to locate a child, or null to search relative to top-level
///                                objects.
/// @param[in] pRelativePathNames  Array of object names comprising the relative path to the target object, starting
///                                from the top-most level.
/// @param[in] pInstanceIndices    Array of object instance indices corresponding to each entry in the object name
///                                array.  This can be null, in which case all indices are assumed to be invalid (no
///                                instance indexing in the object path).
/// @param[in] nameDepth           Depth of the relative path name array.
/// @param[in] packageDepth        Remaining depth into the relative path name array of objects that are packages.
///
/// @return  Pointer to the child object if found, null if not found.
Asset* Asset::FindChildOf(
									const Asset* pObject,
									const Name* pRelativePathNames,
									const uint32_t* pInstanceIndices,
									size_t nameDepth,
									size_t packageDepth )
{
	HELIUM_ASSERT( pRelativePathNames );
	HELIUM_ASSERT( nameDepth >= 1 );

	// Make sure a relative path has been provided.
	if( !pRelativePathNames || nameDepth == 0 )
	{
		return NULL;
	}

	// Search for the direct child of the given object in the path chain.
	Asset* pChild = FindChildOf(
		pObject,
		pRelativePathNames[ 0 ],
		( pInstanceIndices ? pInstanceIndices[ 0 ] : Invalid< uint32_t >() ) );
	if( !pChild )
	{
		return NULL;
	}

	// Verify that the child object is of the correct type (package or non-package).
	if( pChild->m_path.IsPackage() ? packageDepth == 0 : packageDepth != 0 )
	{
		return NULL;
	}

	// Check grandchildren if necessary.
	--nameDepth;
	if( !nameDepth )
	{
		return pChild;
	}

	if( packageDepth )
	{
		--packageDepth;
	}

	pChild = FindChildOf(
		pChild,
		pRelativePathNames + 1,
		( pInstanceIndices ? pInstanceIndices + 1 : NULL ),
		nameDepth,
		packageDepth );

	return pChild;
}

/// Register an Asset instance for object management.
///
/// @param[in] pObject  Asset to register.
///
/// @return  True if the object was registered successfully, false if not (i.e. name clash).
///
/// @see UnregisterObject()
bool Asset::RegisterObject( Asset* pObject )
{
	HELIUM_ASSERT( pObject );

	ScopeWriteLock scopeLock( sm_objectListLock );

	// Check if the object has already been registered.
	if( IsValid( pObject->m_id ) )
	{
		HELIUM_ASSERT( sm_objects.IsElementValid( pObject->m_id ) );
		HELIUM_ASSERT( sm_objects[ pObject->m_id ].Get() == pObject );

		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "Asset::RegisterObject(): Attempted to register object \"%s\", which is already registered.\n" ),
			*pObject->GetPath().ToString() );

		return true;
	}

	HELIUM_ASSERT( pObject->m_name.IsEmpty() );
	HELIUM_ASSERT( !pObject->m_spOwner );
	HELIUM_ASSERT( IsInvalid( pObject->m_instanceIndex ) );

	// Register the object.
	size_t objectId = sm_objects.Add( AssetWPtr( pObject ) );
	HELIUM_ASSERT( objectId < UINT32_MAX );

	pObject->m_id = static_cast< uint32_t >( objectId );

	return true;
}

/// Unregister an Asset instance from object management.
///
/// @param[in] pObject  Asset to unregister.
///
/// @see RegisterObject()
void Asset::UnregisterObject( Asset* pObject )
{
	HELIUM_ASSERT( pObject );

	ScopeWriteLock scopeLock( sm_objectListLock );

	// Check if the object has already been unregistered.
	uint32_t objectId = pObject->m_id;
	if( IsInvalid( objectId ) )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "Asset::UnregisterObject(): Called on object \"%s\", which is already unregistered.\n" ),
			*pObject->GetPath().ToString() );

		return;
	}

	if ( sm_objects.GetSize() ) // will be empty if already shutdown
	{
		HELIUM_ASSERT( sm_objects.IsElementValid( objectId ) );
		HELIUM_ASSERT( sm_objects[ objectId ].HasObjectProxy( pObject ) );

		HELIUM_ASSERT( pObject->m_name.IsEmpty() );
		HELIUM_ASSERT( !pObject->m_spOwner );
		HELIUM_ASSERT( IsInvalid( pObject->m_instanceIndex ) );

		// Remove the object from the global list.
		sm_objects.Remove( objectId );
	}

	SetInvalid( pObject->m_id );
}

/// Perform shutdown of the Asset system.
///
/// This releases all final references to objects and releases all allocated memory.  This should be called during
/// the shutdown process after all types have been unregistered as well as after calling AssetType::Shutdown().
///
/// @see AssetType::Shutdown()
void Asset::Shutdown()
{
	HELIUM_TRACE( TraceLevels::Info, TXT( "Shutting down Asset system.\n" ) );
	
#if !HELIUM_RELEASE
	size_t objectCountActual = sm_objects.GetUsedSize();
	if( objectCountActual != 0 )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "%" ) PRIuSZ TXT( " asset(s) still referenced during shutdown!\n" ),
			objectCountActual );

		size_t objectCount = sm_objects.GetSize();
		for( size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
		{
			if( !sm_objects.IsElementValid( objectIndex ) )
			{
				continue;
			}

			Asset* pObject = sm_objects[ objectIndex ];
			if( !pObject )
			{
				continue;
			}
			
#if HELIUM_ENABLE_MEMORY_TRACKING
			Helium::RefCountProxy<Reflect::Object> *pProxy = pObject->GetRefCountProxy();
			HELIUM_ASSERT(pProxy);

			HELIUM_TRACE(
					TraceLevels::Error,
					TXT( "   - 0x%p: %s (%" ) PRIu16 TXT( " strong ref(s), %" ) PRIu16 TXT( " weak ref(s))\n" ),
					 pProxy,
					( pObject ? *pObject->GetPath().ToString() : TXT( "(cleared reference)" ) ),
					pProxy->GetStrongRefCount(),
					pProxy->GetWeakRefCount() );
#else
			HELIUM_TRACE( TraceLevels::Error, TXT( "- %s\n" ), *pObject->GetPath().ToString() );
#endif
		}
	}
#endif  // !HELIUM_RELEASE

	sm_objects.Clear();
	sm_wpFirstTopLevelObject.Release();

	delete sm_pNameInstanceIndexMap;
	sm_pNameInstanceIndexMap = NULL;

	delete sm_pEmptyNameInstanceIndexMap;
	sm_pEmptyNameInstanceIndexMap = NULL;

	delete sm_pEmptyInstanceIndexSet;
	sm_pEmptyInstanceIndexSet = NULL;

	sm_serializationBuffer.Clear();
}

/// Initialize the static type information for the "Asset" class.
///
/// @return  Static "Asset" type.
const AssetType* Asset::InitStaticType()
{
	HELIUM_ASSERT( s_MetaClass );
	if ( !s_MetaClass->m_Tag )
	{
		// To resolve interdependencies between the Asset type information and other objects (i.e. the owner
		// package, its type, etc.), we will create and register all the dependencies here manually as well.
		Name nameObject( TXT( "Asset" ) );
		Name namePackage( TXT( "Package" ) );

		RenameParameters nameParamsObject, nameParamsPackage, nameParamsEngine, nameParamsTypes;
		nameParamsEngine.name.Set( TXT( "Engine" ) );
		nameParamsTypes.name.Set( TXT( "Types" ) );

		Package* pTypesPackage = new Package();
		HELIUM_ASSERT( pTypesPackage );
		HELIUM_VERIFY( RegisterObject( pTypesPackage ) );
		HELIUM_VERIFY( pTypesPackage->Rename( nameParamsTypes ) );

		AssetType::SetTypePackage( pTypesPackage );

		nameParamsEngine.spOwner = pTypesPackage;

		Package* pEnginePackage = new Package();
		HELIUM_ASSERT( pEnginePackage );
		HELIUM_VERIFY( RegisterObject( pEnginePackage ) );
		HELIUM_VERIFY( pEnginePackage->Rename( nameParamsEngine ) );

		// Don't set up templates here; they're initialized during type registration.
		AssetPtr spObjectTemplate = Helium::Reflect::AssertCast< Asset >( s_MetaClass->m_Default );
		HELIUM_ASSERT( spObjectTemplate );

		PackagePtr spPackageTemplate = new Package();
		HELIUM_ASSERT( spPackageTemplate );

		// Package flag is set automatically by the Package constructor, but it shouldn't be set for the Package
		// type template.
		spPackageTemplate->ClearFlags( FLAG_PACKAGE );

		// Initialize and register all types.
		AssetType::Create(
			Reflect::GetMetaClass< Asset >(),
			pEnginePackage,
			NULL,
			spObjectTemplate,
			AssetType::FLAG_ABSTRACT );

		HELIUM_VERIFY( AssetType::Create(
			Reflect::GetMetaClass< Package >(),
			pEnginePackage,
			static_cast< const AssetType* >( s_MetaClass->m_Tag ),
			spPackageTemplate,
			0 ) );

		// Force initialization of Package so it can report its static type information.
		HELIUM_VERIFY( Package::InitStaticType() );
	}

	return static_cast< const AssetType* >( s_MetaClass->m_Tag );
}

/// Release static type information for this class.
void Asset::ReleaseStaticType()
{
	if( s_MetaClass )
	{
		AssetType::Unregister( static_cast< const AssetType* >( s_MetaClass->m_Tag ) );
		s_MetaClass = NULL;
	}
}

/// Get the static "Asset" type.
///
/// @return  Static "Asset" type.
const AssetType* Asset::GetStaticType()
{
	HELIUM_ASSERT( s_MetaClass );
	return static_cast< const AssetType* >( s_MetaClass->m_Tag );
}

/// Set the custom destruction callback for this object.
///
/// This is used by the object declaration macros and should not be called directly by other code.
///
/// @param[in] pDestroyCallback  Custom destruction callback to set.
void Asset::SetCustomDestroyCallback( CUSTOM_DESTROY_CALLBACK* pDestroyCallback )
{
	m_pCustomDestroyCallback = pDestroyCallback;
}

/// Update the stored path for this object.
///
/// This should be called whenever the name of this object or one of its parents changes.
void Asset::UpdatePath()
{
	// Update this object's path first.
	HELIUM_VERIFY( m_path.Set(
		m_name,
		IsPackage(),
		( m_spOwner ? m_spOwner->m_path : AssetPath( NULL_NAME ) ),
		m_instanceIndex ) );

	// Update the path of each child object.
	for( Asset* pChild = m_wpFirstChild; pChild != NULL; pChild = pChild->m_wpNextSibling )
	{
		pChild->UpdatePath();
	}
}

/// Custom destroy callback for objects created using CreateObject().
///
/// @param[in] pObject  Asset to destroy.
void Asset::StandardCustomDestroy( Asset* pObject )
{
	HELIUM_ASSERT( pObject );
	pObject->InPlaceDestroy();
	Helium::DefaultAllocator allocator;
	allocator.FreeAligned( pObject );
}

/// Get the static name instance lookup map, creating it if necessary.
///
/// Since our hash table implementation dynamically allocates buckets on construction and always keeps them around
/// for the lifetime of the table, we dynamically construct our object name instance map so that we can destroy it
/// during shutdown, ensuring that we no longer have any dynamic allocations within the engine.
///
/// @return  Reference to the name instance lookup map.
Asset::ChildNameInstanceIndexMap& Asset::GetNameInstanceIndexMap()
{
	if( !sm_pNameInstanceIndexMap )
	{
		sm_pNameInstanceIndexMap = new ChildNameInstanceIndexMap;
		HELIUM_ASSERT( sm_pNameInstanceIndexMap );

		HELIUM_ASSERT( !sm_pEmptyNameInstanceIndexMap );
		sm_pEmptyNameInstanceIndexMap = new Pair< AssetPath, NameInstanceIndexMap >;
		HELIUM_ASSERT( sm_pEmptyNameInstanceIndexMap );

		HELIUM_ASSERT( !sm_pEmptyInstanceIndexSet );
		sm_pEmptyInstanceIndexSet = new Pair< Name, InstanceIndexSet >;
		HELIUM_ASSERT( sm_pEmptyInstanceIndexSet );
	}

	return *sm_pNameInstanceIndexMap;
}

AssetRegistrar< Asset, void > Asset::s_Registrar(TXT("Helium::Asset"));


PackagePtr AssetType::sm_spTypePackage;
AssetType::LookupMap* AssetType::sm_pLookupMap = NULL;

/// Constructor.
AssetType::AssetType()
	: m_class( NULL )
	, m_flags( 0 )
{
}

/// Destructor.
AssetType::~AssetType()
{
}

/// Get the default template object for this type.
///
/// @return  Type template object.
Asset* AssetType::GetTemplate() const
{
	return const_cast< Asset* >( static_cast< Asset* >( m_class->m_Default.Get() ) );
}


/// Set the package in which all template object packages are stored.
///
/// @param[in] pPackage  Main type package.
///
/// @see GetTypePackage()
void AssetType::SetTypePackage( Package* pPackage )
{
	HELIUM_ASSERT( pPackage );

	// Only allow the type package to be set once.
	HELIUM_ASSERT( !sm_spTypePackage );

	sm_spTypePackage = pPackage;
}

/// Create a type object.
///
/// @param[in] name          Type name.
/// @param[in] pTypePackage  Package in which the template object should be stored.
/// @param[in] pParent       Parent type.
/// @param[in] pTemplate     Template object.
/// @param[in] flags         Type flags.
///
/// @return  Pointer to the type object if created successfully, null if not.
///
/// @see Unregister()
// PMD: Removing const because:
// - Objects must be able to have properties of the same type as the outer type (i.e. Asset has reference to Asset that is the template)
// - So, s_MetaClass must be set before calling PopulateMetaType
// - So, this function must return a pointer that PopulateMetaType can work on, rather than calling PopulateMetaType directly
//   - If not for this restriction, I'd want to see if we could call MetaClass::Create and Composite::Create, rather than doing duplicate set-up work here
// - To prevent un-consting parameter to PopulateMetaType, making AssetType return non-const
AssetType* AssetType::Create(
	const Reflect::MetaClass* pClass,
	Package* pTypePackage,
	const AssetType* pParent,
	Asset* pTemplate,
	uint32_t flags )
{
	HELIUM_ASSERT( pClass );
	HELIUM_ASSERT( pTypePackage );
	HELIUM_ASSERT( pTemplate );

	Name name;
	name.Set( pClass->m_Name );
	HELIUM_ASSERT( !name.IsEmpty() );

	// Register the template object with the object system.
	if( !Asset::RegisterObject( pTemplate ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "AssetType::Initialize(): Failed to register type \"%s\" template object.\n" ),
			*name );

		return NULL;
	}

	// Set up the template object name, and set this object as its parent.
	Asset::RenameParameters nameParameters;
	nameParameters.name = name;
	nameParameters.spOwner = pTypePackage;
	if( !pTemplate->Rename( nameParameters ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "AssetType::Initialize(): Failed to set type \"%s\" template object name and owner.\n" ),
			*name );

		Asset::UnregisterObject( pTemplate );

		return NULL;
	}

	// Flag the object as the default template object for the type being created.
	pTemplate->SetFlags( 
		Asset::FLAG_DEFAULT_TEMPLATE | 
		Asset::FLAG_TRANSIENT | 
		Asset::FLAG_PRELOADED | 
		Asset::FLAG_LINKED |
		Asset::FLAG_PRECACHED |
		Asset::FLAG_LOADED);

	// Create the type object and store its parameters.
	AssetType* pType = new AssetType;
	HELIUM_ASSERT( pType );
	pType->m_class = pClass;
	pClass->m_Tag = pType;
	const_cast< Reflect::MetaClass* >( pType->m_class )->m_Default = pTemplate;
	const_cast< Reflect::MetaClass* >( pType->m_class )->MetaStruct::m_Default = pTemplate;
	pType->m_name = name;
	pType->m_flags = flags;

	// Lazily initialize the lookup map.  Note that this is not inherently thread-safe, but there should always be
	// at least one type registered before any sub-threads are spawned.
	if( !sm_pLookupMap )
	{
		sm_pLookupMap = new LookupMap;
		HELIUM_ASSERT( sm_pLookupMap );
	}

	// Register the type (note that a type with the same name should not already exist in the lookup map).
	LookupMap::Iterator typeIterator;
	HELIUM_VERIFY( sm_pLookupMap->Insert( typeIterator, KeyValue< Name, AssetTypePtr >( pType->GetName(), pType ) ) );

	return pType;
}

/// Unregister a type.
///
/// Note that this should only be called by the static type release functions (i.e. Entity::ReleaseStaticType()).
///
/// @param[in] pType  Type to unregister.  References to the parent type and the type template will be released as well.
///
/// @see Register()
void AssetType::Unregister( const AssetType* pType )
{
	HELIUM_ASSERT( pType );

	HELIUM_ASSERT( sm_pLookupMap );
	HELIUM_VERIFY( sm_pLookupMap->Remove( pType->GetName() ) );
}

/// Look up a type by name.
///
/// @param[in] typeName  Name of the type to look up.
///
/// @return  Pointer to the specified type if found, null pointer if not found.
AssetType* AssetType::Find( Name typeName )
{
	AssetType* pType = NULL;
	if( sm_pLookupMap )
	{
		LookupMap::ConstIterator typeIterator = sm_pLookupMap->Find( typeName );
		if( typeIterator != sm_pLookupMap->End() )
		{
			pType = typeIterator->Second();
			HELIUM_ASSERT( pType );
		}
	}

	return pType;
}

/// Get an iterator referencing the first registered type.
///
/// @return  Iterator referencing the first registered type.
///
/// @see GetTypeEnd()
AssetType::ConstIterator AssetType::GetTypeBegin()
{
	if( sm_pLookupMap )
	{
		return ConstIterator( sm_pLookupMap->Begin() );
	}

	ConstIterator nullIterator;
	MemoryZero( &nullIterator, sizeof( nullIterator ) );

	return nullIterator;
}

/// Get an iterator referencing the end of the type registration map.
///
/// @return  Iterator referencing the end of the type registration map (one past the last entry).
///
/// @see GetTypeBegin()
AssetType::ConstIterator AssetType::GetTypeEnd()
{
	if( sm_pLookupMap )
	{
		return ConstIterator( sm_pLookupMap->End() );
	}

	ConstIterator nullIterator;
	MemoryZero( &nullIterator, sizeof( nullIterator ) );

	return nullIterator;
}

/// Perform shutdown of the AssetType registration system.
///
/// This releases all final references to objects and releases all allocated memory.  This should be called during
/// the shutdown process prior to calling Asset::Shutdown().
///
/// @see Asset::Shutdown()
void AssetType::Shutdown()
{
	HELIUM_TRACE( TraceLevels::Info, TXT( "Shutting down AssetType registration.\n" ) );

	delete sm_pLookupMap;
	sm_pLookupMap = NULL;

	// Release the reference to the main "Types" package.
	sm_spTypePackage.Release();

	HELIUM_TRACE( TraceLevels::Info, TXT( "AssetType registration shutdown complete.\n" ) );
}

HELIUM_IMPLEMENT_ASSET_NOINITTYPE( Package, Engine );

/// Constructor.
Package::Package()
	: m_pLoader( NULL )
{
	// Set the package flag by default.
	SetFlags( FLAG_PACKAGE );
}

/// Destructor.
Package::~Package()
{
}

/// Initialize the static type information for the "Package" class.
///
/// @return  Static "Package" type.
const AssetType* Package::InitStaticType()
{
	HELIUM_ASSERT( s_MetaClass )
	if ( !s_MetaClass->m_Tag )
	{
		// Package type is registered manually during Asset type initialization, so retrieve the type info from the
		// existing registered data.
		HELIUM_VERIFY( Asset::InitStaticType() );
	}

	return static_cast< const Helium::AssetType* >( s_MetaClass->m_Tag );
}

/// Set the loader for this package.
///
/// @param[in] pLoader  Package loader.
///
/// @see GetLoader()
void Package::SetLoader( PackageLoader* pLoader )
{
	m_pLoader = pLoader;
}
