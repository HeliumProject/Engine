//----------------------------------------------------------------------------------------------------------------------
// Object.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/Object.h"

#include "Engine/Type.h"
#include "Engine/Package.h"
#include "Engine/DirectSerializer.h"
#include "Engine/DirectDeserializer.h"

namespace Lunar
{
    L_IMPLEMENT_REF_COUNT( Object );

    TypeWPtr Object::sm_spStaticType;
    ObjectPtr Object::sm_spStaticTypeTemplate;

    SparseArray< ObjectWPtr > Object::sm_objects;
    DynArray< ObjectWPtr > Object::sm_topLevelObjects;
    Object::ChildNameInstanceIndexMap* Object::sm_pNameInstanceIndexMap = NULL;
    ReadWriteLock Object::sm_objectListLock;

    DynArray< uint8_t > Object::sm_serializationBuffer;

    /// Constructor.
    Object::Object()
        : m_name( NULL_NAME )
        , m_instanceIndex( Invalid< uint32_t >() )
        , m_id( Invalid< uint32_t >() )
        , m_flags( 0 )
        , m_path( NULL_NAME )
        , m_pCustomDestroyCallback( NULL )
    {
    }

    /// Destructor.
    Object::~Object()
    {
        HELIUM_ASSERT_MSG(
            GetAnyFlagSet( Object::FLAG_PREDESTROYED ),
            TXT( "Object::PreDestroy() not called prior to destruction." ) );
    }

    /// Change the name of this object.
    ///
    /// @param[in] name  Name to set.
    ///
    /// @return  True if the name was changed successfully, false if not.
    ///
    /// @see GetName()
    bool Object::SetName( Name name )
    {
        // Don't allow setting an empty name.
        if( name.IsEmpty() )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Cannot set an empty object name.\n" ) );

            return false;
        }

        // Don't need to do anything if the name is not changing.
        if( m_name == name )
        {
            return true;
        }

        // Acquire a write lock on the object list to prevent objects from being added and removed as well as keep
        // objects from being renamed while this object is being renamed.
        ScopeWriteLock scopeLock( sm_objectListLock );

        // Make sure another object with the same name and instance index doesn't already exist.
        Object* pOwner = m_spOwner;
        if( pOwner )
        {
            DynArray< ObjectWPtr >& ownerChildren = pOwner->m_children;
            size_t ownerChildCount = ownerChildren.GetSize();
            for( size_t childIndex = 0; childIndex < ownerChildCount; ++childIndex )
            {
                Object* pChild = ownerChildren[ childIndex ];
                if( pChild && pChild->m_name == name && pChild->m_instanceIndex == m_instanceIndex )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "Cannot rename \"%s\" to \"%s\" (instance %" ) TPRIu32 TXT( ") due to a name conflict " )
                          TXT( "with another object belonging to the same owner.\n" ) ),
                        *GetPath().ToString(),
                        name.Get(),
                        m_instanceIndex );

                    return false;
                }
            }

            // Set the new name.
            m_name = name;

            UpdatePath();

            return true;
        }

        // Search for top-level object name clashes.
        size_t topLevelObjectCount = sm_topLevelObjects.GetSize();
        for( size_t objectIndex = 0; objectIndex < topLevelObjectCount; ++objectIndex )
        {
            Object* pTopLevelObject = sm_topLevelObjects[ objectIndex ];
            if( pTopLevelObject &&
                pTopLevelObject != this &&
                pTopLevelObject->GetName() == name &&
                pTopLevelObject->GetInstanceIndex() == m_instanceIndex )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Cannot rename \"%s\" to \"%s\" (instance %" ) TPRIu32 TXT( ") due to a name conflict " )
                      TXT( "with another top-level object.\n" ) ),
                    *GetPath().ToString(),
                    name.Get(),
                    m_instanceIndex );

                return false;
            }
        }

        // Set the new name.
        RemoveInstanceIndexTracking();
        m_name = name;
        AddInstanceIndexTracking();

        UpdatePath();

        return true;
    }

    /// Change the instance index of this object.
    ///
    /// @param[in] index  Instance index to set.
    ///
    /// @return  True if the instance index was changed successfully, false if not.
    ///
    /// @see GetInstanceIndex()
    bool Object::SetInstanceIndex( uint32_t index )
    {
        // Don't need to do anything if the instance index is not changing.
        if( m_instanceIndex == index )
        {
            return true;
        }

        // Acquire a write lock on the object list to prevent objects from being added and removed as well as keep
        // objects from being renamed while this object is being renamed.
        ScopeWriteLock scopeLock( sm_objectListLock );

        // Make sure another object with the same name and instance index doesn't already exist.
        Object* pOwner = m_spOwner;
        if( pOwner )
        {
            DynArray< ObjectWPtr >& ownerChildren = pOwner->m_children;
            size_t ownerChildCount = ownerChildren.GetSize();
            for( size_t childIndex = 0; childIndex < ownerChildCount; ++childIndex )
            {
                Object* pChild = ownerChildren[ childIndex ];
                if( pChild && pChild->m_name == m_name && pChild->m_instanceIndex == index )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "Cannot change instance index of \"%s\" to %" ) TPRIu32 TXT( " due to a name conflict " )
                          TXT( "with another object belonging to the same owner.\n" ) ),
                        *GetPath().ToString(),
                        index );

                    return false;
                }
            }

            // Set the new instance index.
            RemoveInstanceIndexTracking();
            m_instanceIndex = index;
            AddInstanceIndexTracking();

            UpdatePath();

            return true;
        }

        // Search for top-level object instance index clashes.
        size_t topLevelObjectCount = sm_topLevelObjects.GetSize();
        for( size_t objectIndex = 0; objectIndex < topLevelObjectCount; ++objectIndex )
        {
            Object* pTopLevelObject = sm_topLevelObjects[ objectIndex ];
            if( pTopLevelObject &&
                pTopLevelObject != this &&
                pTopLevelObject->GetName() == m_name &&
                pTopLevelObject->GetInstanceIndex() == index )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Cannot change instance index of \"%s\" to %" ) TPRIu32 TXT( " due to a name conflict " )
                      TXT( "with another top-level object.\n" ) ),
                    *GetPath().ToString(),
                    index );

                return false;
            }
        }

        // Set the new instance index.
        RemoveInstanceIndexTracking();
        m_instanceIndex = index;
        AddInstanceIndexTracking();

        UpdatePath();

        return true;
    }

    /// Set all object flags covered by the given mask.
    ///
    /// Note that all object flag functions are thread-safe.
    ///
    /// @param[in] flagMask  Object flag bit mask.
    ///
    /// @return  Object flag state immediately prior to setting the given flags.
    ///
    /// @see ClearFlags(), ToggleFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
    uint32_t Object::SetFlags( uint32_t flagMask )
    {
        HELIUM_ASSERT( flagMask != 0 );

        return AtomicOr( reinterpret_cast< volatile int32_t& >( m_flags ), flagMask );
    }

    /// Clear all object flags covered by the given mask.
    ///
    /// Note that all object flag functions are thread-safe.
    ///
    /// @param[in] flagMask  Object flag bit mask.
    ///
    /// @return  Object flag state immediately prior to clearing the given flags.
    ///
    /// @see SetFlags(), ToggleFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
    uint32_t Object::ClearFlags( uint32_t flagMask )
    {
        HELIUM_ASSERT( flagMask != 0 );

        return AtomicAnd( reinterpret_cast< volatile int32_t& >( m_flags ), ~flagMask );
    }

    /// Toggle all object flags covered by the given mask.
    ///
    /// Note that all object flag functions are thread-safe.
    ///
    /// @param[in] flagMask  Object flag bit mask.
    ///
    /// @return  Object flag state immediately prior to clearing the given flags.
    ///
    /// @see SetFlags(), ClearFlags(), GetFlags(), GetAnyFlagSet(), GetAllFlagsSet()
    uint32_t Object::ToggleFlags( uint32_t flagMask )
    {
        HELIUM_ASSERT( flagMask != 0 );

        return AtomicXor( reinterpret_cast< volatile int32_t& >( m_flags ), flagMask );
    }

    /// Get the template for this object.
    ///
    /// @return  Object template.
    Object* Object::GetTemplate() const
    {
        Object* pTemplate = m_spTemplate;
        if( !pTemplate )
        {
            Type* pType = GetType();
            HELIUM_ASSERT( pType );
            pTemplate = pType->GetTypeTemplate();
            HELIUM_ASSERT( pTemplate );
        }

        return pTemplate;
    }

    /// Set the owner of this object.
    ///
    /// @param[in] pOwner               Owner to set.
    /// @param[in] bResetInstanceIndex  True to reset the instance index if one is specified, false to attempt to keep
    ///                                 the same index.  Note that this parameter is ignored if the instance index is
    ///                                 invalid, and behavior will be the same as if this flag was set to false.
    ///
    /// @return  True if the owner was switched successfully, false if not.
    ///
    /// @see GetOwner()
    bool Object::SetOwner( Object* pOwner, bool bResetInstanceIndex )
    {
        HELIUM_ASSERT( pOwner != this );
        if( pOwner == this )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Attempted to set owner of \"%s\" to itself.\n" ), *GetPath().ToString() );

            return false;
        }

        Object* pCurrentOwner = m_spOwner;
        if( pOwner == pCurrentOwner )
        {
            // Owner already set...
            return true;
        }

        // Don't reset the instance index if one is not set.
        bResetInstanceIndex = ( bResetInstanceIndex && IsValid( m_instanceIndex ) );

        // Keep track of the old owner for the duration of this function call.  This is done to avoid deadlocks during
        // potentially recursive calls to this function if this object is the last object holding onto a strong
        // reference to it (the Object destructor calls SetOwner(), which can cause a deadlock trying to reacquire an
        // exclusive lock on m_objectListLock).
        ObjectPtr spOldOwner = m_spOwner;

        {
            ScopeWriteLock scopeLock( sm_objectListLock );

            uint32_t newInstanceIndex = m_instanceIndex;

            if( bResetInstanceIndex )
            {
                // Determine the next available instance index for the specified name.
                newInstanceIndex = 0;

                ChildNameInstanceIndexMap& rNameInstanceIndexMap = GetNameInstanceIndexMap();

                ChildNameInstanceIndexMap::ConstAccessor childNameMapAccessor;
                bool bFoundChildMap = rNameInstanceIndexMap.Find(
                    childNameMapAccessor,
                    ( pOwner ? pOwner->GetPath() : ObjectPath( NULL_NAME ) ) );
                if( bFoundChildMap )
                {
                    NameInstanceIndexMap::ConstAccessor indexSetAccessor;
                    if( childNameMapAccessor->second.Find( indexSetAccessor, m_name ) )
                    {
                        const InstanceIndexSet& rIndexSet = indexSetAccessor->second;
                        InstanceIndexSet::ConstAccessor indexAccessor;
                        while( rIndexSet.Find( indexAccessor, newInstanceIndex ) )
                        {
                            ++newInstanceIndex;
                        }
                    }
                }
            }

            if( pOwner )
            {
                // Avoid a looping chain of ownership by making sure the new owner is not a child or grandchild of this
                // object.
                for( Object* pTestOwner = pOwner->GetOwner(); pTestOwner != NULL; pTestOwner = pTestOwner->GetOwner() )
                {
                    if( pTestOwner == this )
                    {
                        HELIUM_TRACE(
                            TRACE_ERROR,
                            TXT( "Attempted to set owner of \"%s\" to sub-object \"%s\".\n" ),
                            *GetPath().ToString(),
                            *pOwner->GetPath().ToString() );

                        return false;
                    }
                }

                // Make sure the new owner does not already have a child with the same name and instance index as this
                // object.
                if( !bResetInstanceIndex )
                {
                    DynArray< ObjectWPtr >& ownerChildren = pOwner->m_children;
                    size_t ownerChildCount = ownerChildren.GetSize();

                    for( size_t childIndex = 0; childIndex < ownerChildCount; ++childIndex )
                    {
                        Object* pChild = ownerChildren[ childIndex ];
                        if( pChild && pChild->m_name == m_name && pChild->m_instanceIndex == m_instanceIndex )
                        {
                            HELIUM_TRACE(
                                TRACE_ERROR,
                                ( TXT( "Cannot set owner of \"%s\" (instance %" ) TPRIu32 TXT( ") to \"%s\" due to a " )
                                  TXT( "name conflict with \"%s\".\n" ) ),
                                *GetPath().ToString(),
                                m_instanceIndex,
                                *pOwner->GetPath().ToString(),
                                *pChild->GetPath().ToString() );

                            return false;
                        }
                    }
                }
            }
            else
            {
                // Make sure the top-level object list doesn't already contain an object with the same name as this
                // object.
                if( !bResetInstanceIndex )
                {
                    size_t topLevelObjectCount = sm_topLevelObjects.GetSize();
                    for( size_t objectIndex = 0; objectIndex < topLevelObjectCount; ++objectIndex )
                    {
                        Object* pTopLevelObject = sm_topLevelObjects[ objectIndex ];
                        if( pTopLevelObject &&
                            pTopLevelObject->GetName() == m_name &&
                            pTopLevelObject->GetInstanceIndex() == m_instanceIndex )
                        {
                            HELIUM_TRACE(
                                TRACE_ERROR,
                                ( TXT( "Cannot clear owner of \"%s\" (instance %" ) TPRIu32 TXT( ") due to a name " )
                                  TXT( "conflict with an existing top-level object.\n" ) ),
                                *GetPath().ToString(),
                                m_instanceIndex );

                            return false;
                        }
                    }
                }
            }

            // Remove this object from the current owner's child list.
            RemoveInstanceIndexTracking();

            if( pCurrentOwner )
            {
                DynArray< ObjectWPtr >& ownerChildren = pCurrentOwner->m_children;
                size_t ownerChildCount = ownerChildren.GetSize();
                for( size_t childIndex = 0; childIndex < ownerChildCount; ++childIndex )
                {
                    if( ownerChildren[ childIndex ] == this )
                    {
                        ownerChildren.RemoveSwap( childIndex );

                        break;
                    }
                }
            }
            else
            {
                size_t topLevelObjectCount = sm_topLevelObjects.GetSize();
                for( size_t objectIndex = 0; objectIndex < topLevelObjectCount; ++objectIndex )
                {
                    if( sm_topLevelObjects[ objectIndex ] == this )
                    {
                        sm_topLevelObjects.RemoveSwap( objectIndex );

                        break;
                    }
                }
            }

            // Assign the new owner.
            m_spOwner = pOwner;
            if( pOwner )
            {
                pOwner->m_children.Add( ObjectWPtr( this ) );
            }
            else if( IsValid( m_id ) )
            {
                // Only add this object to the top-level object list if it is already registered.
                sm_topLevelObjects.Add( ObjectWPtr( this ) );
            }

            AddInstanceIndexTracking();

            UpdatePath();
        }

        return true;
    }

    /// Search for a direct child of this object with the given name.
    ///
    /// @param[in] name           Object name.
    /// @param[in] instanceIndex  Object instance index.
    ///
    /// @return  Pointer to the child object if found, null if not found.
    Object* Object::FindChild( Name name, uint32_t instanceIndex ) const
    {
        return FindChildOf( this, name, instanceIndex );
    }

    /// Perform any necessary work immediately prior to destroying this object.
    ///
    /// Note that the parent-class implementation should always be chained last.
    void Object::PreDestroy()
    {
        if( IsValid( m_id ) )
        {
            UnregisterObject( this );
        }

        SetOwner( NULL );
        SetInstanceIndex( Invalid< uint32_t >() );

        SetFlags( Object::FLAG_PREDESTROYED );
    }

    /// Actually destroy this object.
    ///
    /// This should only be called by the reference counting system once the last strong reference to this object has
    /// been cleared.  It should never be called manually
    void Object::Destroy()
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
    /// @return  Object type.
    Type* Object::GetType() const
    {
        return Object::GetStaticType();
    }

    /// Get whether this object is an instance of the specified type or one of its subtypes.
    ///
    /// @param[in] pType  Type against which to test.
    ///
    /// @return  True if this is an instance of the given type or one of its subtypes, false if not.
    ///
    /// @see GetType(), IsInstanceOf()
    bool Object::IsA( const Type* pType ) const
    {
        const Type* pThisType = GetType();
        HELIUM_ASSERT( pThisType );

        return pThisType->IsSubtypeOf( pType );
    }

    /// Serialize this object.
    ///
    /// @param[in] s  Serializer to use for serialization.
    void Object::Serialize( Serializer& /*s*/ )
    {
    }

    /// Get whether this object requires precaching of resource data during loading.
    ///
    /// @return  True if precaching is necessary, false if not.
    ///
    /// @see BeginPrecacheResourceData()
    bool Object::NeedsPrecacheResourceData() const
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
    bool Object::BeginPrecacheResourceData()
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
    bool Object::TryFinishPrecacheResourceData()
    {
        return true;
    }

    /// Perform any work once the load process for an object and its dependencies has completed.
    ///
    /// This is called once an object has been loaded, linked, and all potentially dependent objects have been loaded
    /// and linked as well.
    void Object::FinalizeLoad()
    {
    }

#if L_EDITOR
    /// Perform any work immediately after saving/caching an object in the editor.
    void Object::PostSave()
    {
    }
#endif  // L_EDITOR

    /// Get whether this object is transient.
    ///
    /// Transient objects are not saved into or loaded from a package stored on disk.  An object is transient if its
    /// type or the types of any of its owners have the Type::FLAG_TRANSIENT flag set, or if it or one of its parents
    /// have the Object::FLAG_TRANSIENT flag set.
    ///
    /// @return  True if this object is transient, false if not.
    bool Object::IsTransient() const
    {
        for( const Object* pObject = this; pObject != NULL && !pObject->IsPackage(); pObject = pObject->GetOwner() )
        {
            if( pObject->GetAnyFlagSet( Object::FLAG_TRANSIENT ) )
            {
                return true;
            }

            Type* pType = pObject->GetType();
            HELIUM_ASSERT( pType );
            if( pType->GetTypeFlags() & Type::FLAG_TRANSIENT )
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
    size_t Object::GetInstanceSize() const
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
    Object* Object::InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const
    {
        HELIUM_ASSERT( pMemory );
        HELIUM_ASSERT( pDestroyCallback );

        Object* pObject = new( pMemory ) Object;
        pObject->SetCustomDestroyCallback( pDestroyCallback );

        return pObject;
    }

    /// Destroy this object instance in-place.
    ///
    /// This should only be called if this object was constructed in-place, either manually or from calling
    /// InPlaceConstructor().
    ///
    /// @see InPlaceConstruct(), GetInstanceSize()
    void Object::InPlaceDestroy()
    {
        this->~Object();
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
    Object* Object::CreateObject( Type* pType, Name name, Object* pOwner, Object* pTemplate, bool bAssignInstanceIndex )
    {
        HELIUM_ASSERT( pType );

        // Get the appropriate template object.
        Object* pObjectTemplate = pTemplate;
        if( pObjectTemplate )
        {
            if( pType->GetTypeFlags() & Type::FLAG_NO_TEMPLATE )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "Object::CreateObject(): Objects of type \"%s\" cannot be used as templates.\n" ),
                    *pType->GetPath().ToString() );

                return NULL;
            }
        }
        else
        {
            pObjectTemplate = pType->GetTypeTemplate();
            HELIUM_ASSERT( pObjectTemplate );
        }

        // Make sure the object template is of the correct type.
        if( !pObjectTemplate->IsInstanceOf( pType ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Object::CreateObject: Template object \"%s\" is not of type \"%s\".\n" ),
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
        Object* pObject = pObjectTemplate->InPlaceConstruct( pObjectMemory, StandardCustomDestroy );
        HELIUM_ASSERT( pObject == pObjectMemory );

        pObject->m_spTemplate = pTemplate;

        // Initialize the object based on its default.
        sm_serializationBuffer.Resize( 0 );
        DirectSerializer templateSerializer( sm_serializationBuffer );
        HELIUM_VERIFY( templateSerializer.Serialize( pObjectTemplate ) );

        DirectDeserializer templateDeserializer( sm_serializationBuffer );
        HELIUM_VERIFY( templateDeserializer.Serialize( pObject ) );

        if( !pObject->SetName( name ) ||
            ( bAssignInstanceIndex && !pObject->SetInstanceIndex( 0 ) ) ||
            !pObject->SetOwner( pOwner ) ||
            !RegisterObject( pObject ) )
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
    Object* Object::FindObject( ObjectPath path )
    {
        // Make sure the path isn't empty.
        if( path.IsEmpty() )
        {
            return NULL;
        }

        // Assemble a list of object names and instance indices, from the top level on down.
        size_t pathDepth = 0;
        size_t packageDepth = 0;
        for( ObjectPath testPath = path; !testPath.IsEmpty(); testPath = testPath.GetParent() )
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
        for( ObjectPath testPath = path; !testPath.IsEmpty(); testPath = testPath.GetParent() )
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
    /// @param[in] pObject        Object for which to locate a child, or null to search through top-level objects.
    /// @param[in] name           Object name.
    /// @param[in] instanceIndex  Object instance index.
    ///
    /// @return  Pointer to the child object if found, null if not found.
    Object* Object::FindChildOf( const Object* pObject, Name name, uint32_t instanceIndex )
    {
        HELIUM_ASSERT( !name.IsEmpty() );
        if( name.IsEmpty() )
        {
            return NULL;
        }

        ScopeReadLock scopeLock( sm_objectListLock );

        const DynArray< ObjectWPtr >& rChildren = ( pObject ? pObject->m_children : sm_topLevelObjects );

        size_t childCount = rChildren.GetSize();
        for( size_t childIndex = 0; childIndex < childCount; ++childIndex )
        {
            Object* pChild = rChildren[ childIndex ];
            if( pChild && pChild->GetName() == name && pChild->GetInstanceIndex() == instanceIndex )
            {
                return pChild;
            }
        }

        return NULL;
    }

    /// Search for a child or grandchild of the given object with a relative path dictated by the given parameters.
    ///
    /// @param[in] pObject             Object for which to locate a child, or null to search relative to top-level
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
    Object* Object::FindChildOf(
        const Object* pObject,
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
        Object* pChild = FindChildOf(
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

    /// Register an Object instance for object management.
    ///
    /// @param[in] pObject  Object to register.
    ///
    /// @return  True if the object was registered successfully, false if not (i.e. name clash).
    ///
    /// @see UnregisterObject()
    bool Object::RegisterObject( Object* pObject )
    {
        HELIUM_ASSERT( pObject );

        ScopeWriteLock scopeLock( sm_objectListLock );

        // Check if the object has already been registered.
        if( IsValid( pObject->m_id ) )
        {
            HELIUM_ASSERT( sm_objects.IsElementValid( pObject->m_id ) );
            HELIUM_ASSERT( sm_objects[ pObject->m_id ] == pObject );

            HELIUM_TRACE(
                TRACE_WARNING,
                TXT( "Object::RegisterObject(): Attempted to register object \"%s\", which is already registered.\n" ),
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
                Object* pTopLevelObject = sm_topLevelObjects[ objectIndex ];
                if( pTopLevelObject &&
                    pTopLevelObject->GetName() == objectName &&
                    pTopLevelObject->GetInstanceIndex() == objectInstanceIndex )
                {
                    HELIUM_ASSERT_MSG_FALSE( TXT( "Cannot register top-level object due to name clash." ) );

                    return false;
                }
            }

            sm_topLevelObjects.Add( ObjectWPtr( pObject ) );
        }

        // Register the object.
        size_t objectId = sm_objects.Add( ObjectWPtr( pObject ) );
        HELIUM_ASSERT( objectId < UINT32_MAX );

        pObject->m_id = static_cast< uint32_t >( objectId );

        return true;
    }

    /// Unregister an Object instance from object management.
    ///
    /// @param[in] pObject  Object to unregister.
    ///
    /// @see RegisterObject()
    void Object::UnregisterObject( Object* pObject )
    {
        HELIUM_ASSERT( pObject );

        ScopeWriteLock scopeLock( sm_objectListLock );

        // Check if the object has already been unregistered.
        uint32_t objectId = pObject->m_id;
        if( IsInvalid( objectId ) )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                TXT( "Object::UnregisterObject(): Called on object \"%s\", which is already unregistered.\n" ),
                *pObject->GetPath().ToString() );

            return;
        }

        HELIUM_ASSERT( sm_objects.IsElementValid( objectId ) );
        HELIUM_ASSERT( sm_objects[ objectId ].HasObjectProxy( pObject ) );

        // If the object is a top-level object, remove it from the top-level object list.
        if( !pObject->GetOwner() )
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

    /// Perform shutdown of the Object system.
    ///
    /// This releases all final references to objects and releases all allocated memory.  This should be called during
    /// the shutdown process after all types have been unregistered as well as after calling Type::Shutdown().
    ///
    /// @see Type::Shutdown()
    void Object::Shutdown()
    {
        HELIUM_TRACE( TRACE_INFO, TXT( "Shutting down Object system.\n" ) );

        Object::ReleaseStaticType();

#if HELIUM_ENABLE_MEMORY_TRACKING
        ConcurrentHashSet< RefCountProxy< Object >* >::ConstAccessor refCountProxyAccessor;
        if( ObjectRefCountSupport::GetFirstActiveProxy( refCountProxyAccessor ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "%" ) TPRIuSZ TXT( " smart pointer(s) still active during shutdown!\n" ),
                ObjectRefCountSupport::GetActiveProxyCount() );

            while( refCountProxyAccessor.IsValid() )
            {
                RefCountProxy< Object >* pProxy = *refCountProxyAccessor;
                HELIUM_ASSERT( pProxy );

                Object* pObject = pProxy->GetObject();

                HELIUM_TRACE(
                    TRACE_ERROR,
                    TXT( "- 0x%p: %s (%" ) TPRIu16 TXT( " strong ref(s), %" ) TPRIu16 TXT( " weak ref(s))\n" ),
                    pProxy,
                    ( pObject ? *pObject->GetPath().ToString() : TXT( "(cleared reference)" ) ),
                    pProxy->GetStrongRefCount(),
                    pProxy->GetWeakRefCount() );

                ++refCountProxyAccessor;
            }
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

                Object* pObject = sm_objects[ objectIndex ];
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

        sm_serializationBuffer.Clear();
    }

    /// Initialize the static type information for the "Object" class.
    ///
    /// @return  Static "Object" type.
    Type* Object::InitStaticType()
    {
        Type* pObjectType = sm_spStaticType;
        if( !pObjectType )
        {
            // To resolve interdependencies between the Object type information and other objects (i.e. the owner
            // package, its type, etc.), we will create and register all the dependencies here manually as well.
            Name nameObject( TXT( "Object" ) );
            Name nameType( TXT( "Type" ) );
            Name namePackage( TXT( "Package" ) );
            Name nameEngine( TXT( "Engine" ) );
            Name nameTypes( TXT( "Types" ) );

            Package* pTypesPackage = new Package();
            HELIUM_ASSERT( pTypesPackage );
            HELIUM_VERIFY( pTypesPackage->SetName( nameTypes ) );
            HELIUM_VERIFY( RegisterObject( pTypesPackage ) );

            Type::SetTypePackage( pTypesPackage );

            Package* pEnginePackage = new Package();
            HELIUM_ASSERT( pEnginePackage );
            HELIUM_VERIFY( pEnginePackage->SetName( nameEngine ) );
            HELIUM_VERIFY( pEnginePackage->SetOwner( pTypesPackage ) );
            HELIUM_VERIFY( RegisterObject( pEnginePackage ) );

            pObjectType = new Type();
            HELIUM_ASSERT( pObjectType );
            pObjectType->SetName( nameObject );
            pObjectType->SetOwner( pEnginePackage );
            pObjectType->SetFlags( Object::FLAG_TRANSIENT );
            HELIUM_VERIFY( RegisterObject( pObjectType ) );
            pObjectType->SetFlags( Object::FLAG_PRELOADED | Object::FLAG_LINKED | Object::FLAG_LOADED );

            Type* pTypeType = new Type();
            HELIUM_ASSERT( pTypeType );
            pTypeType->SetName( nameType );
            pTypeType->SetOwner( pEnginePackage );
            pTypeType->SetFlags( Object::FLAG_TRANSIENT );
            HELIUM_VERIFY( RegisterObject( pTypeType ) );
            pTypeType->SetFlags( Object::FLAG_PRELOADED | Object::FLAG_LINKED | Object::FLAG_LOADED );

            Type* pPackageType = new Type();
            HELIUM_ASSERT( pPackageType );
            pPackageType->SetName( namePackage );
            pPackageType->SetOwner( pEnginePackage );
            pPackageType->SetFlags( Object::FLAG_TRANSIENT );
            HELIUM_VERIFY( RegisterObject( pPackageType ) );
            pPackageType->SetFlags( Object::FLAG_PRELOADED | Object::FLAG_LINKED | Object::FLAG_LOADED );

            // Don't set up templates here; they're initialized during type registration.
            Object* pObjectTemplate = new Object();
            HELIUM_ASSERT( pObjectTemplate );

            Type* pTypeTemplate = new Type();
            HELIUM_ASSERT( pTypeTemplate );

            Package* pPackageTemplate = new Package();
            HELIUM_ASSERT( pPackageTemplate );

            // Package flag is set automatically by the Package constructor, but it shouldn't be set for the Package
            // type template.
            pPackageTemplate->ClearFlags( FLAG_PACKAGE );

            // Initialize and register all types.
            HELIUM_VERIFY( pObjectType->Initialize( NULL, pObjectTemplate, Type::FLAG_ABSTRACT ) );
            HELIUM_VERIFY( Type::Register( pObjectType ) );

            HELIUM_VERIFY( pTypeType->Initialize( pObjectType, pTypeTemplate, 0 ) );
            HELIUM_VERIFY( Type::Register( pTypeType ) );

            HELIUM_VERIFY( pPackageType->Initialize( pObjectType, pPackageTemplate, 0 ) );
            HELIUM_VERIFY( Type::Register( pPackageType ) );

            sm_spStaticType = pObjectType;

            // Force initialization of Type and Package so they can report their static types.
            HELIUM_VERIFY( Type::InitStaticType() );
            HELIUM_VERIFY( Package::InitStaticType() );
        }

        return pObjectType;
    }

    /// Release static type information for this class.
    void Object::ReleaseStaticType()
    {
        Type* pType = sm_spStaticType;
        if( pType )
        {
            Type::Unregister( pType );
        }

        sm_spStaticType.Release();
        sm_spStaticTypeTemplate.Release();
    }

    /// Get the static "Object" type.
    ///
    /// @return  Static "Object" type.
    Type* Object::GetStaticType()
    {
        HELIUM_ASSERT( sm_spStaticType );
        return sm_spStaticType;
    }

    /// Set the custom destruction callback for this object.
    ///
    /// This is used by the object declaration macros and should not be called directly by other code.
    ///
    /// @param[in] pDestroyCallback  Custom destruction callback to set.
    void Object::SetCustomDestroyCallback( CUSTOM_DESTROY_CALLBACK* pDestroyCallback )
    {
        m_pCustomDestroyCallback = pDestroyCallback;
    }

    /// Register tracking information for the instance index associated with this object.
    ///
    /// @see RemoveInstanceIndexTracking()
    void Object::AddInstanceIndexTracking()
    {
        if( IsInvalid( m_instanceIndex ) )
        {
            return;
        }

        ObjectPath ownerPath = ( m_spOwner ? m_spOwner->GetPath() : ObjectPath( NULL_NAME ) );

        std::pair< ObjectPath, NameInstanceIndexMap > childMapEntry;
        childMapEntry.first = ownerPath;

        std::pair< Name, InstanceIndexSet > indexSetEntry;
        indexSetEntry.first = m_name;

        ChildNameInstanceIndexMap& rNameInstanceIndexMap = GetNameInstanceIndexMap();

        ChildNameInstanceIndexMap::Accessor childMapAccessor;
        rNameInstanceIndexMap.Insert( childMapAccessor, childMapEntry );

        NameInstanceIndexMap::Accessor nameMapAccessor;
        childMapAccessor->second.Insert( nameMapAccessor, indexSetEntry );

        InstanceIndexSet::Accessor indexSetAccessor;
        HELIUM_VERIFY( nameMapAccessor->second.Insert( indexSetAccessor, m_instanceIndex ) );
    }

    /// Remove the tracking information for the instance index associated with this object.
    ///
    /// @see AddInstanceIndexTracking()
    void Object::RemoveInstanceIndexTracking()
    {
        if( IsInvalid( m_instanceIndex ) )
        {
            return;
        }

        ObjectPath ownerPath = ( m_spOwner ? m_spOwner->GetPath() : ObjectPath( NULL_NAME ) );

        ChildNameInstanceIndexMap& rNameInstanceIndexMap = GetNameInstanceIndexMap();

        ChildNameInstanceIndexMap::Accessor childMapAccessor;
        HELIUM_VERIFY( rNameInstanceIndexMap.Find( childMapAccessor, ownerPath ) );

        NameInstanceIndexMap& rNameMap = childMapAccessor->second;
        NameInstanceIndexMap::Accessor nameMapAccessor;
        HELIUM_VERIFY( rNameMap.Find( nameMapAccessor, m_name ) );

        InstanceIndexSet& rIndexSet = nameMapAccessor->second;
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
    void Object::UpdatePath()
    {
        // Update this object's path first.
        HELIUM_VERIFY( m_path.Set(
            m_name,
            IsPackage(),
            ( m_spOwner ? m_spOwner->m_path : ObjectPath( NULL_NAME ) ),
            m_instanceIndex ) );

        // Update the path of each child object.
        size_t childCount = m_children.GetSize();
        for( size_t childIndex = 0; childIndex < childCount; ++childIndex )
        {
            Object* pObject = m_children[ childIndex ];
            if( pObject )
            {
                pObject->UpdatePath();
            }
        }
    }

    /// Custom destroy callback for objects created using CreateObject().
    ///
    /// @param[in] pObject  Object to destroy.
    void Object::StandardCustomDestroy( Object* pObject )
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
    Object::ChildNameInstanceIndexMap& Object::GetNameInstanceIndexMap()
    {
        if( !sm_pNameInstanceIndexMap )
        {
            sm_pNameInstanceIndexMap = new ChildNameInstanceIndexMap;
            HELIUM_ASSERT( sm_pNameInstanceIndexMap );
        }

        return *sm_pNameInstanceIndexMap;
    }
}
