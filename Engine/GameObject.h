//----------------------------------------------------------------------------------------------------------------------
// GameObject.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_OBJECT_H
#define LUNAR_ENGINE_OBJECT_H

#include "Foundation/Container/ConcurrentHashMap.h"
#include "Foundation/Container/ConcurrentHashSet.h"
#include "Foundation/Container/DynArray.h"
#include "Foundation/Container/SparseArray.h"
#include "Foundation/Memory/ReferenceCounting.h"
#include "Engine/GameObjectPath.h"

/// @defgroup objectmacros Common "GameObject"-class Macros
//@{

/// Utility macro for declaring standard GameObject-class variables and functions.
///
/// @param[in] TYPE    GameObject type.
/// @param[in] PARENT  Parent object type.
#define L_DECLARE_OBJECT( TYPE, PARENT ) \
    private: \
        static Lunar::TypeWPtr sm_spStaticType; \
        static Lunar::StrongPtr< TYPE > sm_spStaticTypeTemplate; \
    public: \
        typedef PARENT Super; \
        virtual Lunar::Type* GetType() const; \
        virtual size_t GetInstanceSize() const; \
        virtual Lunar::GameObject* InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const; \
        virtual void InPlaceDestroy(); \
        static Lunar::Type* InitStaticType(); \
        static void ReleaseStaticType(); \
        static Lunar::Type* GetStaticType();

/// Utility macro for implementing standard GameObject-class variables and functions, without implementing InitStaticType().
///
/// @param[in] TYPE    GameObject type.
/// @param[in] MODULE  Module to which the type belongs.
#define L_IMPLEMENT_OBJECT_NOINITTYPE( TYPE, MODULE ) \
    Lunar::TypeWPtr TYPE::sm_spStaticType; \
    Lunar::StrongPtr< TYPE > TYPE::sm_spStaticTypeTemplate; \
    \
    Lunar::Type* TYPE::GetType() const \
    { \
        return TYPE::GetStaticType(); \
    } \
    \
    size_t TYPE::GetInstanceSize() const \
    { \
        return sizeof( *this ); \
    } \
    \
    Lunar::GameObject* TYPE::InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const \
    { \
        HELIUM_ASSERT( pMemory ); \
        HELIUM_ASSERT( pDestroyCallback ); \
        \
        TYPE* pObject = new( pMemory ) TYPE; \
        pObject->SetCustomDestroyCallback( pDestroyCallback ); \
        \
        return pObject; \
    } \
    \
    void TYPE::InPlaceDestroy() \
    { \
        this->~TYPE(); \
    } \
    \
    void TYPE::ReleaseStaticType() \
    { \
        Lunar::Type* pType = sm_spStaticType; \
        if( pType ) \
        { \
            Lunar::Type::Unregister( pType ); \
        } \
        \
        sm_spStaticType.Release(); \
        sm_spStaticTypeTemplate.Release(); \
    } \
    \
    Lunar::Type* TYPE::GetStaticType() \
    { \
        HELIUM_ASSERT( sm_spStaticType ); \
        return sm_spStaticType; \
    }

/// Utility macro for implementing standard GameObject-class variables and functions.
///
/// @param[in] TYPE        GameObject type.
/// @param[in] MODULE      Module to which the type belongs.
/// @param[in] TYPE_FLAGS  Type flags.
#define L_IMPLEMENT_OBJECT( TYPE, MODULE, TYPE_FLAGS ) \
    L_IMPLEMENT_OBJECT_NOINITTYPE( TYPE, MODULE ) \
    \
    Lunar::Type* TYPE::InitStaticType() \
    { \
        Lunar::Type* pType = sm_spStaticType; \
        if( !pType ) \
        { \
            HELIUM_ASSERT( !sm_spStaticTypeTemplate ); \
            \
            extern Lunar::Package* Get##MODULE##TypePackage(); \
            Lunar::Package* pTypePackage = Get##MODULE##TypePackage(); \
            HELIUM_ASSERT( pTypePackage ); \
            \
            Lunar::Type* pParentType = Super::InitStaticType(); \
            HELIUM_ASSERT( pParentType ); \
            \
            TYPE* pTemplate = new TYPE; \
            HELIUM_ASSERT( pTemplate ); \
            sm_spStaticTypeTemplate = pTemplate; \
            \
            pType = Lunar::Type::Create( \
                Lunar::Name( TXT( #TYPE ) ), \
                pTypePackage, \
                pParentType, \
                pTemplate, \
                TYPE_FLAGS ); \
            HELIUM_ASSERT( pType ); \
            sm_spStaticType = pType; \
        } \
        \
        return pType; \
    }

//@}

namespace Lunar
{
    class Serializer;

    HELIUM_DECLARE_PTR( GameObject );
    HELIUM_DECLARE_PTR( Type );
    HELIUM_DECLARE_PTR( Package );

    HELIUM_DECLARE_WPTR( GameObject );
    HELIUM_DECLARE_WPTR( Type );

    /// Reference counting support for GameObject types.
    class LUNAR_ENGINE_API GameObjectRefCountSupport
    {
    public:
        /// Base type of reference counted object.
        typedef GameObject BaseType;

        /// @name Object Destruction Support
        //@{
        inline static void PreDestroy( GameObject* pObject );
        inline static void Destroy( GameObject* pObject );
        //@}

        /// @name Reference Count Proxy Allocation Interface
        //@{
        static RefCountProxy< GameObject >* Allocate();
        static void Release( RefCountProxy< GameObject >* pProxy );

        static void Shutdown();
        //@}

#if HELIUM_ENABLE_MEMORY_TRACKING
        /// @name Active Proxy Iteration
        //@{
        static size_t GetActiveProxyCount();
        static bool GetFirstActiveProxy(
            ConcurrentHashSet< RefCountProxy< GameObject >* >::ConstAccessor& rAccessor );
        //@}
#endif

    private:
        struct StaticData;

        /// Static proxy management data.
        static StaticData* sm_pStaticData;
    };

    /// Base class for the engine's game object system.
    class LUNAR_ENGINE_API GameObject : NonCopyable
    {
        HELIUM_DECLARE_REF_COUNT( GameObject, GameObjectRefCountSupport );

    public:
        /// Destruction callback type.
        typedef void ( CUSTOM_DESTROY_CALLBACK )( GameObject* pObject );

        /// Object flags.
        enum EFlag
        {
            /// Object property data has been loaded, but object is not ready for use.
            FLAG_PRELOADED        = 1 << 0,
            /// Object references have been linked.
            FLAG_LINKED           = 1 << 1,
            /// Object resource data has been precached.
            FLAG_PRECACHED        = 1 << 2,
            /// Object loading has completed (object and all its dependencies are ready for use).
            FLAG_LOADED           = 1 << 3,

            /// Object has gone through pre-destruction cleanup.
            FLAG_PREDESTROYED     = 1 << 4,

            /// Object is broken.
            FLAG_BROKEN           = 1 << 5,

            /// Object is the default template for its type.
            FLAG_DEFAULT_TEMPLATE = 1 << 6,
            /// Object and its children are transient.
            FLAG_TRANSIENT        = 1 << 7,
            /// Object is a package (only set for Package objects *excluding* the Package type template).
            FLAG_PACKAGE          = 1 << 8
        };

        /// @name Construction/Destruction
        //@{
        GameObject();
        virtual ~GameObject();
        //@}

        /// @name GameObject Interface
        //@{
        inline Name GetName() const;
        bool SetName( Name name );

        inline uint32_t GetInstanceIndex() const;
        bool SetInstanceIndex( uint32_t index );

        inline uint32_t GetId() const;

        inline uint32_t GetFlags() const;
        inline bool GetAnyFlagSet( uint32_t flagMask ) const;
        inline bool GetAllFlagsSet( uint32_t flagMask ) const;
        uint32_t SetFlags( uint32_t flagMask );
        uint32_t ClearFlags( uint32_t flagMask );
        uint32_t ToggleFlags( uint32_t flagMask );

        GameObject* GetTemplate() const;

        inline GameObject* GetOwner() const;
        bool SetOwner( GameObject* pOwner, bool bResetInstanceIndex = true );

        inline size_t GetChildCount() const;
        inline GameObject* GetChild( size_t index ) const;
        inline const DynArray< GameObjectWPtr >& GetChildren() const;
        GameObject* FindChild( Name name, uint32_t instanceIndex = Invalid< uint32_t >() ) const;

        inline GameObjectPath GetPath() const;

        inline bool IsFullyLoaded() const;
        inline bool IsDefaultTemplate() const;
        inline bool IsPackage() const;

        virtual void PreDestroy();

        // This should only be called by the reference counting system!
        void Destroy();
        //@}

        /// @name RTTI
        //@{
        virtual Type* GetType() const;
        bool IsA( const Type* pType ) const;
        inline bool IsInstanceOf( const Type* pType ) const;
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );

        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();

        virtual void FinalizeLoad();
        inline void ConditionalFinalizeLoad();

#if L_EDITOR
        virtual void PostSave();
#endif

        bool IsTransient() const;
        //@}

        /// @name Creation Utility Functions
        //@{
        virtual size_t GetInstanceSize() const;
        virtual GameObject* InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const;
        virtual void InPlaceDestroy();
        //@}

        /// @name GameObject Management
        //@{
        static GameObject* CreateObject(
            Type* pType, Name name, GameObject* pOwner, GameObject* pTemplate = NULL, bool bAssignInstanceIndex = false );
        template< typename T > static T* Create(
            Name name, GameObject* pOwner, T* pTemplate = NULL, bool bAssignInstanceIndex = false );

        static GameObject* FindObject( GameObjectPath path );
        template< typename T > static T* Find( GameObjectPath path );

        static GameObject* FindChildOf( const GameObject* pObject, Name name, uint32_t instanceIndex = Invalid< uint32_t >() );
        static GameObject* FindChildOf(
            const GameObject* pObject, const Name* pRelativePathNames, const uint32_t* pInstanceIndices, size_t nameDepth,
            size_t packageDepth );

        static bool RegisterObject( GameObject* pObject );
        static void UnregisterObject( GameObject* pObject );

        static void Shutdown();
        //@}

        /// @name Static Interface
        //@{
        static Type* InitStaticType();
        static void ReleaseStaticType();
        static Type* GetStaticType();
        //@}

    protected:
        /// @name Creation Utility Functions, Protected
        //@{
        void SetCustomDestroyCallback( CUSTOM_DESTROY_CALLBACK* pDestroyCallback );
        //@}

    private:
        /// Name instance index lookup set type.
        typedef ConcurrentHashSet< uint32_t > InstanceIndexSet;
        /// Name instance lookup map type.
        typedef ConcurrentHashMap< Name, InstanceIndexSet > NameInstanceIndexMap;
        /// Child object name instance lookup map type.
        typedef ConcurrentHashMap< GameObjectPath, NameInstanceIndexMap > ChildNameInstanceIndexMap;

        /// Object name.
        Name m_name;
        /// Instance index.
        uint32_t m_instanceIndex;
        /// Object ID.
        uint32_t m_id;
        /// Object flags.
        volatile uint32_t m_flags;
        /// Override object template (null if using the type's default object).
        GameObjectPtr m_spTemplate;

        /// Object owner.
        GameObjectPtr m_spOwner;
        /// Object children.
        DynArray< GameObjectWPtr > m_children;

        /// Full object path name.
        GameObjectPath m_path;

        /// Custom callback for notifying that this object should be destroyed when its reference count drops to zero
        /// (provided for custom object allocation schemes).
        CUSTOM_DESTROY_CALLBACK* m_pCustomDestroyCallback;

        /// Static "GameObject" type instance.
        static TypeWPtr sm_spStaticType;
        /// Static "GameObject" template instance.
        static GameObjectPtr sm_spStaticTypeTemplate;

        /// Global object list.
        static SparseArray< GameObjectWPtr > sm_objects;
        /// Top-level object list.
        static DynArray< GameObjectWPtr > sm_topLevelObjects;
        /// Name instance lookup.
        static ChildNameInstanceIndexMap* sm_pNameInstanceIndexMap;
        /// Read-write lock for synchronizing access to the object lists.
        static ReadWriteLock sm_objectListLock;

        /// Cached serialization buffer.
        static DynArray< uint8_t > sm_serializationBuffer;

        /// @name Private Utility Functions
        //@{
        void AddInstanceIndexTracking();
        void RemoveInstanceIndexTracking();

        void UpdatePath();
        //@}

        /// @name Reference Counting Support, Private
        //@{
        static void StandardCustomDestroy( GameObject* pObject );
        //@}

        /// @name Static GameObject Management
        //@{
        static ChildNameInstanceIndexMap& GetNameInstanceIndexMap();
        //@}
    };

    /// @defgroup objectcast Type-checking GameObject Casting Functions
    //@{
    template< typename TargetType, typename SourceType > TargetType* DynamicCast( SourceType* pObject );
    template< typename TargetType, typename SourceType > TargetType* StaticCast( SourceType* pObject );
    //@}
}

#include "Engine/GameObject.inl"

#endif  // LUNAR_ENGINE_OBJECT_H
