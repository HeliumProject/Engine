//----------------------------------------------------------------------------------------------------------------------
// GameObject.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/GameObject.h"

#include "Foundation/Container/ObjectPool.h"
#include "Engine/GameObjectType.h"
#include "Engine/Package.h"
#include "Engine/DirectSerializer.h"
#include "Engine/DirectDeserializer.h"

using namespace Lunar;

REFLECT_DEFINE_CLASS( GameObject )

const GameObjectType* GameObject::sm_pStaticType = NULL;

SparseArray< GameObjectWPtr > GameObject::sm_objects;
DynArray< GameObjectWPtr > GameObject::sm_topLevelObjects;

GameObject::ChildNameInstanceIndexMap* GameObject::sm_pNameInstanceIndexMap = NULL;
Pair< GameObjectPath, GameObject::NameInstanceIndexMap >* GameObject::sm_pEmptyNameInstanceIndexMap = NULL;
Pair< Name, GameObject::InstanceIndexSet >* GameObject::sm_pEmptyInstanceIndexSet = NULL;

ReadWriteLock GameObject::sm_objectListLock;

DynArray< uint8_t > GameObject::sm_serializationBuffer;

/// Constructor.
GameObject::GameObject()
    : m_name( NULL_NAME )
    , m_instanceIndex( Invalid< uint32_t >() )
    , m_id( Invalid< uint32_t >() )
    , m_flags( 0 )
    , m_path( NULL_NAME )
    , m_pCustomDestroyCallback( NULL )
{
}

/// Destructor.
GameObject::~GameObject()
{
    HELIUM_ASSERT_MSG(
        GetAnyFlagSet( GameObject::FLAG_PREDESTROYED ),
        TXT( "GameObject::PreDestroy() not called prior to destruction." ) );
}

/// Modify the name, owner, or instance index of this object.
///
/// @param[in] rParameters  Object rename parameters.
///
/// @return  True if this object was renamed successfully, false if not.
///
/// @see GetName(), GetOwner(), GetInstanceIndex()
bool GameObject::Rename( const RenameParameters& rParameters )
{
    Name name = rParameters.name;
    GameObject* pOwner = rParameters.spOwner;
    uint32_t instanceIndex = rParameters.instanceIndex;

    // Only allow setting an empty name if no owner or instance index are given and this object has no children.
    if( name.IsEmpty() )
    {
        HELIUM_ASSERT( !pOwner );
        HELIUM_ASSERT( IsInvalid( instanceIndex ) );
        if( pOwner || IsValid( instanceIndex ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "GameObject::Rename(): Objects cannot have name information cleared if being assigned an " )
                  TXT( "owner or instance index.\n" ) ) );

            return false;
        }

        HELIUM_ASSERT( m_children.IsEmpty() );
        if( !m_children.IsEmpty() )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "GameObject::Rename(): Cannot clear name information for objects with children.\n" ) );

            return false;
        }
    }

    // Don't allow setting the owner to ourself.
    if( pOwner == this )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameObject::Rename(): Cannot set the owner of an object to itself.\n" ) );

        return false;
    }

    // Don't allow setting the owner to an object with no name information.
    if( pOwner && pOwner->m_name.IsEmpty() )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "GameObject::Rename(): Cannot set the owner of an object to an object with no path information.\n" ) );

        return false;
    }

    if( IsPackage() )
    {
        // Don't allow package objects to be children of non-package objects.
        if( pOwner && !pOwner->IsPackage() )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "GameObject::Rename(): Cannot set a non-package as the owner of a package.\n" ) );

            return false;
        }

        // Don't allow instance indexing for packages.
        if( IsValid( instanceIndex ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "GameObject::Rename(): Instance indexing not supported for packages.\n" ) );

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
    GameObjectPtr spOldOwner = m_spOwner;

    {
        // Acquire a write lock on the object list to prevent objects from being added and removed as well as keep
        // objects from being renamed while this object is being renamed.
        ScopeWriteLock scopeLock( sm_objectListLock );

        // Get the list of children belonging to the new owner.
        DynArray< GameObjectWPtr >& rOwnerChildren = ( pOwner ? pOwner->m_children : sm_topLevelObjects );

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

                sm_pEmptyNameInstanceIndexMap->First() = ( pOwner ? pOwner->GetPath() : GameObjectPath( NULL_NAME ) );
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
                            TRACE_ERROR,
                            ( TXT( "GameObject::Rename(): Object already exists with the specified owner (%s), name " )
                              TXT( "(%s), and instance index (%" ) TPRIu32 TXT( ").\n" ) ),
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
                size_t ownerChildCount = rOwnerChildren.GetSize();
                for( size_t childIndex = 0; childIndex < ownerChildCount; ++childIndex )
                {
                    GameObject* pChild = rOwnerChildren[ childIndex ];
                    if( pChild && pChild->m_name == name && pChild->m_instanceIndex == instanceIndex )
                    {
                        HELIUM_TRACE(
                            TRACE_ERROR,
                            ( TXT( "GameObject::Rename(): Object already exists with the specified owner (%s) and " )
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
            GameObjectPath ownerPath = ( spOldOwner ? spOldOwner->GetPath() : GameObjectPath( NULL_NAME ) );

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
        if( spOldOwner.Get() != pOwner )
        {
            // Object should not be in any child object lists if its name is empty.
            if( !m_name.IsEmpty() )
            {
                DynArray< GameObjectWPtr >& rOldOwnerChildren =
                    ( spOldOwner ? spOldOwner->m_children : sm_topLevelObjects );
                size_t ownerChildCount = rOldOwnerChildren.GetSize();
                for( size_t childIndex = 0; childIndex < ownerChildCount; ++childIndex )
                {
                    if( rOldOwnerChildren[ childIndex ].Get() == this )
                    {
                        rOldOwnerChildren.RemoveSwap( childIndex );
                        break;
                    }
                }
            }

            // Only store the object in a child object list if it is being given a valid name.
            if( !name.IsEmpty() )
            {
                rOwnerChildren.Push( this );
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

/// Set all object flags covered by the given mask.
///
/// Note that all object flag functions are thread-safe.
///
/// @param[in] flagMask  GameObject flag bit mask.
///
/// @return  GameObject flag state immediately prior to setting the given flags.
///
/// @see ClearFlags(), ToggleFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
uint32_t GameObject::SetFlags( uint32_t flagMask )
{
    HELIUM_ASSERT( flagMask != 0 );

    return AtomicOr( reinterpret_cast< volatile int32_t& >( m_flags ), flagMask );
}

/// Clear all object flags covered by the given mask.
///
/// Note that all object flag functions are thread-safe.
///
/// @param[in] flagMask  GameObject flag bit mask.
///
/// @return  GameObject flag state immediately prior to clearing the given flags.
///
/// @see SetFlags(), ToggleFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
uint32_t GameObject::ClearFlags( uint32_t flagMask )
{
    HELIUM_ASSERT( flagMask != 0 );

    return AtomicAnd( reinterpret_cast< volatile int32_t& >( m_flags ), ~flagMask );
}

/// Toggle all object flags covered by the given mask.
///
/// Note that all object flag functions are thread-safe.
///
/// @param[in] flagMask  GameObject flag bit mask.
///
/// @return  GameObject flag state immediately prior to clearing the given flags.
///
/// @see SetFlags(), ClearFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
uint32_t GameObject::ToggleFlags( uint32_t flagMask )
{
    HELIUM_ASSERT( flagMask != 0 );

    return AtomicXor( reinterpret_cast< volatile int32_t& >( m_flags ), flagMask );
}

/// Get the template for this object.
///
/// @return  GameObject template.
GameObject* GameObject::GetTemplate() const
{
    GameObject* pTemplate = m_spTemplate;
    if( !pTemplate )
    {
        const GameObjectType* pType = GetGameObjectType();
        HELIUM_ASSERT( pType );
        pTemplate = pType->GetTemplate();
        HELIUM_ASSERT( pTemplate );
    }

    return pTemplate;
}

/// Search for a direct child of this object with the given name.
///
/// @param[in] name           GameObject name.
/// @param[in] instanceIndex  GameObject instance index.
///
/// @return  Pointer to the child object if found, null if not found.
GameObject* GameObject::FindChild( Name name, uint32_t instanceIndex ) const
{
    return FindChildOf( this, name, instanceIndex );
}

/// @copydoc Object::PreDestroy()
void GameObject::PreDestroy()
{
    if( IsValid( m_id ) )
    {
        UnregisterObject( this );
    }

    HELIUM_VERIFY( Rename( RenameParameters() ) );

    SetFlags( GameObject::FLAG_PREDESTROYED );
}

/// @copydoc Object::Destroy()
void GameObject::Destroy()
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
/// @return  GameObject type.
const GameObjectType* GameObject::GetGameObjectType() const
{
    return GameObject::GetStaticType();
}

/// Serialize this object.
///
/// @param[in] s  Serializer to use for serialization.
void GameObject::Serialize( Serializer& /*s*/ )
{
}

/// Get whether this object requires precaching of resource data during loading.
///
/// @return  True if precaching is necessary, false if not.
///
/// @see BeginPrecacheResourceData()
bool GameObject::NeedsPrecacheResourceData() const
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
bool GameObject::BeginPrecacheResourceData()
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
bool GameObject::TryFinishPrecacheResourceData()
{
    return true;
}

/// Perform any work once the load process for an object and its dependencies has completed.
///
/// This is called once an object has been loaded, linked, and all potentially dependent objects have been loaded
/// and linked as well.
void GameObject::FinalizeLoad()
{
}

#if L_EDITOR
/// Perform any work immediately after saving/caching an object in the editor.
void GameObject::PostSave()
{
}
#endif  // L_EDITOR

/// Get whether this object is transient.
///
/// Transient objects are not saved into or loaded from a package stored on disk.  An object is transient if its
/// type or the types of any of its owners have the GameObjectType::FLAG_TRANSIENT flag set, or if it or one of its
/// parents have the GameObject::FLAG_TRANSIENT flag set.
///
/// @return  True if this object is transient, false if not.
bool GameObject::IsTransient() const
{
    for( const GameObject* pObject = this; pObject != NULL && !pObject->IsPackage(); pObject = pObject->GetOwner() )
    {
        if( pObject->GetAnyFlagSet( GameObject::FLAG_TRANSIENT ) )
        {
            return true;
        }

        const GameObjectType* pType = pObject->GetGameObjectType();
        HELIUM_ASSERT( pType );
        if( pType->GetFlags() & GameObjectType::FLAG_TRANSIENT )
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
size_t GameObject::GetInstanceSize() const
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
GameObject* GameObject::InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const
{
    HELIUM_ASSERT( pMemory );
    HELIUM_ASSERT( pDestroyCallback );

    GameObject* pObject = new( pMemory ) GameObject;
    pObject->SetCustomDestroyCallback( pDestroyCallback );

    return pObject;
}

/// Destroy this object instance in-place.
///
/// This should only be called if this object was constructed in-place, either manually or from calling
/// InPlaceConstructor().
///
/// @see InPlaceConstruct(), GetInstanceSize()
void GameObject::InPlaceDestroy()
{
    this->~GameObject();
}

/// Create a new object.
///
/// @param[in] pType                 Type of object to create.
/// @param[in] name                  Object name.
/// @param[in] pOwner                Object owner.
/// @param[in] pTemplate             Optional override template object.  If null, the default template for the
///                                  specified type will be used.
/// @param[in] bAssignInstanceIndex  True to assign an instance index to the object, false to leave the index
///                                  invalid.
///
/// @return  Pointer to the newly created object.
///
/// @see Create()
GameObject* GameObject::CreateObject(
    const GameObjectType* pType,
    Name name,
    GameObject* pOwner,
    GameObject* pTemplate,
    bool bAssignInstanceIndex )
{
    HELIUM_ASSERT( pType );

    // Get the appropriate template object.
    GameObject* pObjectTemplate = pTemplate;
    if( pObjectTemplate )
    {
        if( pType->GetFlags() & GameObjectType::FLAG_NO_TEMPLATE && pType->GetTemplate() != pObjectTemplate )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "GameObject::CreateObject(): Objects of type \"%s\" cannot be used as templates.\n" ),
                *pType->GetName() );

            return NULL;
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
            TRACE_ERROR,
            TXT( "GameObject::CreateObject: Template object \"%s\" is not of type \"%s\".\n" ),
            *pTemplate->GetPath().ToString(),
            pType->GetName().Get() );
        HELIUM_ASSERT_FALSE();

        return NULL;
    }

    // Allocate memory for and create the object.
    DefaultAllocator allocator;

    size_t bufferSize = pObjectTemplate->GetInstanceSize();
    void* pObjectMemory = allocator.AllocateAligned( HELIUM_SIMD_ALIGNMENT, bufferSize );
    HELIUM_ASSERT( pObjectMemory );
    GameObject* pObject = pObjectTemplate->InPlaceConstruct( pObjectMemory, StandardCustomDestroy );
    HELIUM_ASSERT( pObject == pObjectMemory );

    pObject->m_spTemplate = pTemplate;

    // Initialize the object based on its default.
    sm_serializationBuffer.Resize( 0 );
    DirectSerializer templateSerializer( sm_serializationBuffer );
    HELIUM_VERIFY( templateSerializer.Serialize( pObjectTemplate ) );

    DirectDeserializer templateDeserializer( sm_serializationBuffer );
    HELIUM_VERIFY( templateDeserializer.Serialize( pObject ) );

    RenameParameters nameParameters;
    nameParameters.name = name;
    nameParameters.spOwner = pOwner;
    if( bAssignInstanceIndex )
    {
        nameParameters.instanceIndex = INSTANCE_INDEX_AUTO;
    }

    if( !pObject->Rename( nameParameters ) || !RegisterObject( pObject ) )
    {
        HELIUM_ASSERT_FALSE();
        pObject->InPlaceDestroy();
        allocator.Free( pObjectMemory );

        return NULL;
    }

    return pObject;
}

/// Find an object based on its path name.
///
/// @param[in] path  Path of the object to locate.
///
/// @return  Pointer to the object if found, null pointer if not found.
GameObject* GameObject::FindObject( GameObjectPath path )
{
    // Make sure the path isn't empty.
    if( path.IsEmpty() )
    {
        return NULL;
    }

    // Assemble a list of object names and instance indices, from the top level on down.
    size_t pathDepth = 0;
    size_t packageDepth = 0;
    for( GameObjectPath testPath = path; !testPath.IsEmpty(); testPath = testPath.GetParent() )
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
    for( GameObjectPath testPath = path; !testPath.IsEmpty(); testPath = testPath.GetParent() )
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
/// @param[in] pObject        GameObject for which to locate a child, or null to search through top-level objects.
/// @param[in] name           GameObject name.
/// @param[in] instanceIndex  GameObject instance index.
///
/// @return  Pointer to the child object if found, null if not found.
GameObject* GameObject::FindChildOf( const GameObject* pObject, Name name, uint32_t instanceIndex )
{
    HELIUM_ASSERT( !name.IsEmpty() );
    if( name.IsEmpty() )
    {
        return NULL;
    }

    ScopeReadLock scopeLock( sm_objectListLock );

    const DynArray< GameObjectWPtr >& rChildren = ( pObject ? pObject->m_children : sm_topLevelObjects );

    size_t childCount = rChildren.GetSize();
    for( size_t childIndex = 0; childIndex < childCount; ++childIndex )
    {
        GameObject* pChild = rChildren[ childIndex ];
        if( pChild && pChild->GetName() == name && pChild->GetInstanceIndex() == instanceIndex )
        {
            return pChild;
        }
    }

    return NULL;
}

/// Search for a child or grandchild of the given object with a relative path dictated by the given parameters.
///
/// @param[in] pObject             GameObject for which to locate a child, or null to search relative to top-level
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
GameObject* GameObject::FindChildOf(
                                    const GameObject* pObject,
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
    GameObject* pChild = FindChildOf(
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

/// Register an GameObject instance for object management.
///
/// @param[in] pObject  GameObject to register.
///
/// @return  True if the object was registered successfully, false if not (i.e. name clash).
///
/// @see UnregisterObject()
bool GameObject::RegisterObject( GameObject* pObject )
{
    HELIUM_ASSERT( pObject );

    ScopeWriteLock scopeLock( sm_objectListLock );

    // Check if the object has already been registered.
    if( IsValid( pObject->m_id ) )
    {
        HELIUM_ASSERT( sm_objects.IsElementValid( pObject->m_id ) );
        HELIUM_ASSERT( sm_objects[ pObject->m_id ].Get() == pObject );

        HELIUM_TRACE(
            TRACE_WARNING,
            TXT( "GameObject::RegisterObject(): Attempted to register object \"%s\", which is already registered.\n" ),
            *pObject->GetPath().ToString() );

        return true;
    }

    // Make sure the object has a name.
    Name objectName = pObject->GetName();
    if( objectName.IsEmpty() )
    {
        HELIUM_ASSERT_MSG_FALSE( TXT( "Cannot register an object with an empty name." ) );

        return false;
    }

    // If the object is a top-level object, make sure its name doesn't clash with any existing top-level object
    // names.
    uint32_t objectInstanceIndex = pObject->GetInstanceIndex();

    if( !pObject->GetOwner() )
    {
        size_t topLevelObjectCount = sm_topLevelObjects.GetSize();
        for( size_t objectIndex = 0; objectIndex < topLevelObjectCount; ++objectIndex )
        {
            GameObject* pTopLevelObject = sm_topLevelObjects[ objectIndex ];
            if( pTopLevelObject &&
                pTopLevelObject->GetName() == objectName &&
                pTopLevelObject->GetInstanceIndex() == objectInstanceIndex )
            {
                HELIUM_ASSERT_MSG_FALSE( TXT( "Cannot register top-level object due to name clash." ) );

                return false;
            }
        }

        sm_topLevelObjects.Add( GameObjectWPtr( pObject ) );
    }

    // Register the object.
    size_t objectId = sm_objects.Add( GameObjectWPtr( pObject ) );
    HELIUM_ASSERT( objectId < UINT32_MAX );

    pObject->m_id = static_cast< uint32_t >( objectId );

    return true;
}

/// Unregister an GameObject instance from object management.
///
/// @param[in] pObject  GameObject to unregister.
///
/// @see RegisterObject()
void GameObject::UnregisterObject( GameObject* pObject )
{
    HELIUM_ASSERT( pObject );

    ScopeWriteLock scopeLock( sm_objectListLock );

    // Check if the object has already been unregistered.
    uint32_t objectId = pObject->m_id;
    if( IsInvalid( objectId ) )
    {
        HELIUM_TRACE(
            TRACE_WARNING,
            TXT( "GameObject::UnregisterObject(): Called on object \"%s\", which is already unregistered.\n" ),
            *pObject->GetPath().ToString() );

        return;
    }

    HELIUM_ASSERT( sm_objects.IsElementValid( objectId ) );
    HELIUM_ASSERT( sm_objects[ objectId ].HasObjectProxy( pObject ) );

    // If the object is a top-level object, remove it from the top-level object list (note that objects without names
    // will not be in the top-level object list).
    if( !pObject->m_name.IsEmpty() && !pObject->m_spOwner )
    {
        size_t topLevelObjectCount = sm_topLevelObjects.GetSize();
        size_t objectIndex;
        for( objectIndex = 0; objectIndex < topLevelObjectCount; ++objectIndex )
        {
            if( sm_topLevelObjects[ objectIndex ].HasObjectProxy( pObject ) )
            {
                sm_topLevelObjects.RemoveSwap( objectIndex );

                break;
            }
        }

        HELIUM_ASSERT( objectIndex < topLevelObjectCount );
    }

    // Remove the object from the global list.
    sm_objects.Remove( objectId );
    SetInvalid( pObject->m_id );
}

/// Perform shutdown of the GameObject system.
///
/// This releases all final references to objects and releases all allocated memory.  This should be called during
/// the shutdown process after all types have been unregistered as well as after calling GameObjectType::Shutdown().
///
/// @see GameObjectType::Shutdown()
void GameObject::Shutdown()
{
    HELIUM_TRACE( TRACE_INFO, TXT( "Shutting down GameObject system.\n" ) );

    GameObject::ReleaseStaticType();

#pragma TODO( "Fix support for casting between Reflect::Object and GameObject once the type systems have been properly integrated." )
#if HELIUM_ENABLE_MEMORY_TRACKING
    ConcurrentHashSet< RefCountProxy< Reflect::Object >* >::ConstAccessor refCountProxyAccessor;
    if( Reflect::ObjectRefCountSupport::GetFirstActiveProxy( refCountProxyAccessor ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "%" ) TPRIuSZ TXT( " smart pointer(s) still active during shutdown!\n" ),
            Reflect::ObjectRefCountSupport::GetActiveProxyCount() );

#if 1
        refCountProxyAccessor.Release();
#else
        size_t activeGameObjectCount = 0;
        while( refCountProxyAccessor.IsValid() )
        {
            RefCountProxy< Reflect::Object >* pProxy = *refCountProxyAccessor;
            HELIUM_ASSERT( pProxy );

            GameObject* pGameObject = Reflect::ObjectCast< GameObject >( pProxy->GetObject() );
            if( pGameObject )
            {
                ++activeGameObjectCount;
            }

            ++refCountProxyAccessor;
        }

        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "%" ) TPRIuSZ TXT( " active GameObject smart pointer(s):\n" ),
            activeGameObjectCount );

        Reflect::ObjectRefCountSupport::GetFirstActiveProxy( refCountProxyAccessor );
        while( refCountProxyAccessor.IsValid() )
        {
            RefCountProxy< Reflect::Object >* pProxy = *refCountProxyAccessor;
            HELIUM_ASSERT( pProxy );

            GameObject* pGameObject = Reflect::ObjectCast< GameObject >( pProxy->GetObject() );
            if( pGameObject )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "- 0x%p: %s (%" ) TPRIu16 TXT( " strong ref(s), %" ) TPRIu16 TXT( " weak ref(s))\n" ),
                    pProxy,
                    ( pGameObject ? *pGameObject->GetPath().ToString() : TXT( "(cleared reference)" ) ),
                    pProxy->GetStrongRefCount(),
                    pProxy->GetWeakRefCount() );
            }

            ++refCountProxyAccessor;
        }
#endif
    }
#endif  // HELIUM_ENABLE_MEMORY_TRACKING

#if !L_RELEASE
    size_t objectCountActual = sm_objects.GetUsedSize();
    if( objectCountActual != 0 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "%" ) TPRIuSZ TXT( " object(s) still referenced during shutdown!\n" ),
            objectCountActual );

        size_t objectCount = sm_objects.GetSize();
        for( size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex )
        {
            if( !sm_objects.IsElementValid( objectIndex ) )
            {
                continue;
            }

            GameObject* pObject = sm_objects[ objectIndex ];
            if( !pObject )
            {
                continue;
            }

            HELIUM_TRACE( TRACE_ERROR, TXT( "- %s\n" ), *pObject->GetPath().ToString() );
        }
    }
#endif  // !L_RELEASE

    sm_objects.Clear();
    sm_topLevelObjects.Clear();

    delete sm_pNameInstanceIndexMap;
    sm_pNameInstanceIndexMap = NULL;

    delete sm_pEmptyNameInstanceIndexMap;
    sm_pEmptyNameInstanceIndexMap = NULL;

    delete sm_pEmptyInstanceIndexSet;
    sm_pEmptyInstanceIndexSet = NULL;

    sm_serializationBuffer.Clear();
}

/// Initialize the static type information for the "GameObject" class.
///
/// @return  Static "GameObject" type.
const GameObjectType* GameObject::InitStaticType()
{
    if( !sm_pStaticType )
    {
        // To resolve interdependencies between the GameObject type information and other objects (i.e. the owner
        // package, its type, etc.), we will create and register all the dependencies here manually as well.
        Name nameObject( TXT( "GameObject" ) );
        Name namePackage( TXT( "Package" ) );

        RenameParameters nameParamsObject, nameParamsPackage, nameParamsEngine, nameParamsTypes;
        nameParamsEngine.name.Set( TXT( "Engine" ) );
        nameParamsTypes.name.Set( TXT( "Types" ) );

        Package* pTypesPackage = new Package();
        HELIUM_ASSERT( pTypesPackage );
        HELIUM_VERIFY( pTypesPackage->Rename( nameParamsTypes ) );
        HELIUM_VERIFY( RegisterObject( pTypesPackage ) );

        GameObjectType::SetTypePackage( pTypesPackage );

        nameParamsEngine.spOwner = pTypesPackage;

        Package* pEnginePackage = new Package();
        HELIUM_ASSERT( pEnginePackage );
        HELIUM_VERIFY( pEnginePackage->Rename( nameParamsEngine ) );
        HELIUM_VERIFY( RegisterObject( pEnginePackage ) );

        // Don't set up templates here; they're initialized during type registration.
        GameObjectPtr spObjectTemplate = new GameObject();
        HELIUM_ASSERT( spObjectTemplate );

        PackagePtr spPackageTemplate = new Package();
        HELIUM_ASSERT( spPackageTemplate );

        // Package flag is set automatically by the Package constructor, but it shouldn't be set for the Package
        // type template.
        spPackageTemplate->ClearFlags( FLAG_PACKAGE );

        // Initialize and register all types.
        sm_pStaticType = GameObjectType::Create(
            nameObject,
            pEnginePackage,
            NULL,
            spObjectTemplate,
            GameObject::ReleaseStaticType,
            GameObjectType::FLAG_ABSTRACT );
        HELIUM_ASSERT( sm_pStaticType );
        s_Class = sm_pStaticType;

        const GameObjectType* pPackageType = GameObjectType::Create(
            namePackage,
            pEnginePackage,
            sm_pStaticType,
            spPackageTemplate,
            Package::ReleaseStaticType,
            0 );
        HELIUM_ASSERT( pPackageType );

        // Force initialization of Package so it can report its static type information.
        HELIUM_VERIFY( Package::InitStaticType() );
    }

    return sm_pStaticType;
}

/// Release static type information for this class.
void GameObject::ReleaseStaticType()
{
    if( sm_pStaticType )
    {
        GameObjectType::Unregister( sm_pStaticType );
        sm_pStaticType = NULL;
        s_Class = NULL;
    }
}

/// Get the static "GameObject" type.
///
/// @return  Static "GameObject" type.
const GameObjectType* GameObject::GetStaticType()
{
    HELIUM_ASSERT( sm_pStaticType );
    return sm_pStaticType;
}

/// Set the custom destruction callback for this object.
///
/// This is used by the object declaration macros and should not be called directly by other code.
///
/// @param[in] pDestroyCallback  Custom destruction callback to set.
void GameObject::SetCustomDestroyCallback( CUSTOM_DESTROY_CALLBACK* pDestroyCallback )
{
    m_pCustomDestroyCallback = pDestroyCallback;
}

/// Register tracking information for the instance index associated with this object.
///
/// @see RemoveInstanceIndexTracking()
void GameObject::AddInstanceIndexTracking()
{
    if( IsInvalid( m_instanceIndex ) )
    {
        return;
    }

    GameObjectPath ownerPath = ( m_spOwner ? m_spOwner->GetPath() : GameObjectPath( NULL_NAME ) );

    Pair< GameObjectPath, NameInstanceIndexMap > childMapEntry;
    childMapEntry.First() = ownerPath;

    Pair< Name, InstanceIndexSet > indexSetEntry;
    indexSetEntry.First() = m_name;

    ChildNameInstanceIndexMap& rNameInstanceIndexMap = GetNameInstanceIndexMap();

    ChildNameInstanceIndexMap::Accessor childMapAccessor;
    rNameInstanceIndexMap.Insert( childMapAccessor, childMapEntry );

    NameInstanceIndexMap::Accessor nameMapAccessor;
    childMapAccessor->Second().Insert( nameMapAccessor, indexSetEntry );

    InstanceIndexSet::Accessor indexSetAccessor;
    HELIUM_VERIFY( nameMapAccessor->Second().Insert( indexSetAccessor, m_instanceIndex ) );
}

/// Remove the tracking information for the instance index associated with this object.
///
/// @see AddInstanceIndexTracking()
void GameObject::RemoveInstanceIndexTracking()
{
    if( IsInvalid( m_instanceIndex ) )
    {
        return;
    }

    GameObjectPath ownerPath = ( m_spOwner ? m_spOwner->GetPath() : GameObjectPath( NULL_NAME ) );

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

/// Update the stored path for this object.
///
/// This should be called whenever the name of this object or one of its parents changes.
void GameObject::UpdatePath()
{
    // Update this object's path first.
    HELIUM_VERIFY( m_path.Set(
        m_name,
        IsPackage(),
        ( m_spOwner ? m_spOwner->m_path : GameObjectPath( NULL_NAME ) ),
        m_instanceIndex ) );

    // Update the path of each child object.
    size_t childCount = m_children.GetSize();
    for( size_t childIndex = 0; childIndex < childCount; ++childIndex )
    {
        GameObject* pObject = m_children[ childIndex ];
        if( pObject )
        {
            pObject->UpdatePath();
        }
    }
}

/// Custom destroy callback for objects created using CreateObject().
///
/// @param[in] pObject  GameObject to destroy.
void GameObject::StandardCustomDestroy( GameObject* pObject )
{
    HELIUM_ASSERT( pObject );
    pObject->InPlaceDestroy();
    DefaultAllocator().Free( pObject );
}

/// Get the static name instance lookup map, creating it if necessary.
///
/// Since our hash table implementation dynamically allocates buckets on construction and always keeps them around
/// for the lifetime of the table, we dynamically construct our object name instance map so that we can destroy it
/// during shutdown, ensuring that we no longer have any dynamic allocations within the engine.
///
/// @return  Reference to the name instance lookup map.
GameObject::ChildNameInstanceIndexMap& GameObject::GetNameInstanceIndexMap()
{
    if( !sm_pNameInstanceIndexMap )
    {
        sm_pNameInstanceIndexMap = new ChildNameInstanceIndexMap;
        HELIUM_ASSERT( sm_pNameInstanceIndexMap );

        HELIUM_ASSERT( !sm_pEmptyNameInstanceIndexMap );
        sm_pEmptyNameInstanceIndexMap = new Pair< GameObjectPath, NameInstanceIndexMap >;
        HELIUM_ASSERT( sm_pEmptyNameInstanceIndexMap );

        HELIUM_ASSERT( !sm_pEmptyInstanceIndexSet );
        sm_pEmptyInstanceIndexSet = new Pair< Name, InstanceIndexSet >;
        HELIUM_ASSERT( sm_pEmptyInstanceIndexSet );
    }

    return *sm_pNameInstanceIndexMap;
}
