#pragma once

#include "Engine/Engine.h"
#include "Foundation/Reflect/Object.h"

#include "Foundation/Container/ConcurrentHashMap.h"
#include "Foundation/Container/ConcurrentHashSet.h"
#include "Foundation/Container/DynArray.h"
#include "Foundation/Container/SparseArray.h"
#include "Foundation/Memory/ReferenceCounting.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Engine/GameObjectPath.h"

/// @defgroup objectmacros Common "GameObject"-class Macros
//@{

/// Utility macro for declaring standard GameObject-class variables and functions.
///
/// @param[in] TYPE    GameObject type.
/// @param[in] PARENT  Parent object type.
#define HELIUM_DECLARE_OBJECT( TYPE, PARENT ) \
        REFLECT_DECLARE_OBJECT( TYPE, PARENT ) \
    public: \
        virtual const Helium::GameObjectType* GetGameObjectType() const; \
        virtual size_t GetInstanceSize() const; \
        virtual Helium::GameObject* InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const; \
        virtual void InPlaceDestroy(); \
        static const Helium::GameObjectType* InitStaticType(); \
        static void ReleaseStaticType(); \
        static const Helium::GameObjectType* GetStaticType();

/// Utility macro for implementing standard GameObject-class variables and functions, without implementing
/// InitStaticType().
///
/// @param[in] TYPE    GameObject type.
/// @param[in] MODULE  Module to which the type belongs.
#define HELIUM_IMPLEMENT_OBJECT_NOINITTYPE( TYPE, MODULE ) \
    REFLECT_DEFINE_OBJECT( TYPE ) \
    \
    const Helium::GameObjectType* TYPE::GetGameObjectType() const \
    { \
        return TYPE::GetStaticType(); \
    } \
    \
    size_t TYPE::GetInstanceSize() const \
    { \
        return sizeof( *this ); \
    } \
    \
    Helium::GameObject* TYPE::InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const \
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
        if( s_Class ) \
        { \
            Helium::GameObjectType::Unregister( static_cast< const Helium::GameObjectType* >( s_Class->m_Tag ) ); \
            s_Class = NULL; \
        } \
    } \
    \
    const Helium::GameObjectType* TYPE::GetStaticType() \
    { \
        HELIUM_ASSERT( s_Class ); \
        return static_cast< const Helium::GameObjectType* >( s_Class->m_Tag ); \
    }

/// Utility macro for implementing standard GameObject-class variables and functions.
///
/// @param[in] TYPE        GameObject type.
/// @param[in] MODULE      Module to which the type belongs.
/// @param[in] TYPE_FLAGS  Type flags.
#define HELIUM_IMPLEMENT_OBJECT( TYPE, MODULE, TYPE_FLAGS ) \
    HELIUM_IMPLEMENT_OBJECT_NOINITTYPE( TYPE, MODULE ) \
    \
    const Helium::GameObjectType* TYPE::InitStaticType() \
    { \
        HELIUM_ASSERT( s_Class ); \
        if ( !s_Class->m_Tag ) \
        { \
            extern Helium::Package* Get##MODULE##TypePackage(); \
            Helium::Package* pTypePackage = Get##MODULE##TypePackage(); \
            HELIUM_ASSERT( pTypePackage ); \
            \
            const Helium::GameObjectType* pParentType = Base::InitStaticType(); \
            HELIUM_ASSERT( pParentType ); \
            \
            Helium::StrongPtr< TYPE > spTemplate = Helium::Reflect::AssertCast< TYPE >( s_Class->m_Default ); \
            HELIUM_ASSERT( spTemplate ); \
            \
            Helium::GameObjectType::Create( \
                Reflect::GetClass< TYPE >(), \
                pTypePackage, \
                pParentType, \
                spTemplate, \
                TYPE_FLAGS ); \
            HELIUM_ASSERT( s_Class ); \
        } \
        \
        return static_cast< const Helium::GameObjectType* >( s_Class->m_Tag ); \
    }

//@}

namespace Helium
{
    class Serializer;

    class GameObjectType;
    typedef SmartPtr< GameObjectType > GameObjectTypePtr;

    HELIUM_DECLARE_PTR( GameObject );
    HELIUM_DECLARE_WPTR( GameObject );

    /// Base class for the engine's game object system.
    class HELIUM_ENGINE_API GameObject : public Helium::Reflect::Object
    {
        REFLECT_DECLARE_OBJECT( GameObject, Reflect::Object )

    public:
        /// Destruction callback type.
        typedef void ( CUSTOM_DESTROY_CALLBACK )( GameObject* pObject );

        /// Reserved instance index value for auto-assigning an instance index during Rename() calls.
        static const uint32_t INSTANCE_INDEX_AUTO = static_cast< uint32_t >( -2 );

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

        /// Object rename parameters.
        struct HELIUM_ENGINE_API RenameParameters
        {
            /// Object name.
            Name name;
            /// Owner.
            GameObjectPtr spOwner;
            /// Instance index (invalid index value for no instance index, INSTANCE_INDEX_AUTO to auto-assign).
            uint32_t instanceIndex;

            /// @name Construction/Destruction
            //@{
            inline RenameParameters();
            //@}
        };

        /// @name Construction/Destruction
        //@{
        GameObject();
        virtual ~GameObject();
        //@}

        /// @name GameObject Interface
        //@{
        inline Name GetName() const;
        inline GameObject* GetOwner() const;
        inline uint32_t GetInstanceIndex() const;
        bool Rename( const RenameParameters& rParameters );

        inline uint32_t GetId() const;

        inline uint32_t GetFlags() const;
        inline bool GetAnyFlagSet( uint32_t flagMask ) const;
        inline bool GetAllFlagsSet( uint32_t flagMask ) const;
        uint32_t SetFlags( uint32_t flagMask );
        uint32_t ClearFlags( uint32_t flagMask );
        uint32_t ToggleFlags( uint32_t flagMask );

        Reflect::ObjectPtr GetTemplate() const;

        inline const GameObjectWPtr& GetFirstChild() const;
        inline const GameObjectWPtr& GetNextSibling() const;
        GameObject* FindChild( Name name, uint32_t instanceIndex = Invalid< uint32_t >() ) const;

        inline GameObjectPath GetPath() const;

        inline bool IsFullyLoaded() const;
        inline bool IsDefaultTemplate() const;
        inline bool IsPackage() const;

        virtual void PreDestroy();
        void Destroy();  // This should only be called by the reference counting system!
        //@}

        /// @name RTTI
        //@{
        virtual const GameObjectType* GetGameObjectType() const;
        inline bool IsInstanceOf( const GameObjectType* pType ) const;
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );

        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();

        virtual void FinalizeLoad();
        inline void ConditionalFinalizeLoad();

#if HELIUM_TOOLS
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
        static bool CreateObject(
            GameObjectPtr& rspObject, const GameObjectType* pType, Name name, GameObject* pOwner,
            GameObject* pTemplate = NULL, bool bAssignInstanceIndex = false );
        template< typename T > static bool Create(
            StrongPtr< T >& rspObject, Name name, GameObject* pOwner, T* pTemplate = NULL,
            bool bAssignInstanceIndex = false );

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
        static const GameObjectType* InitStaticType();
        static void ReleaseStaticType();
        static const GameObjectType* GetStaticType();
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
        /// First child object.
        GameObjectWPtr m_wpFirstChild;
        /// Next sibling object.
        GameObjectWPtr m_wpNextSibling;

        /// Full object path name.
        GameObjectPath m_path;

        /// Custom callback for notifying that this object should be destroyed when its reference count drops to zero
        /// (provided for custom object allocation schemes).
        CUSTOM_DESTROY_CALLBACK* m_pCustomDestroyCallback;

        /// Global object list.
        static SparseArray< GameObjectWPtr > sm_objects;
        /// First object in the list of top-level objects.
        static GameObjectWPtr sm_wpFirstTopLevelObject;

        /// Name instance lookup.
        static ChildNameInstanceIndexMap* sm_pNameInstanceIndexMap;
        /// Empty object name instance map.
        static Pair< GameObjectPath, NameInstanceIndexMap >* sm_pEmptyNameInstanceIndexMap;
        /// Empty name instance index lookup set.
        static Pair< Name, InstanceIndexSet >* sm_pEmptyInstanceIndexSet;

        /// Read-write lock for synchronizing access to the object lists.
        static ReadWriteLock sm_objectListLock;

        /// Cached serialization buffer.
        static DynArray< uint8_t > sm_serializationBuffer;

        /// @name Private Utility Functions
        //@{
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
