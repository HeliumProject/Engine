#pragma once

#include "Engine/Engine.h"

#include "Foundation/ConcurrentHashMap.h"
#include "Foundation/ConcurrentHashSet.h"
#include "Foundation/DynamicArray.h"
#include "Foundation/HashMap.h"
#include "Foundation/SparseArray.h"
#include "Foundation/ReferenceCounting.h"
#include "Foundation/SmartPtr.h"
#include "Reflect/Object.h"

#include "Engine/AssetPath.h"

/// @defgroup objectmacros Common "Asset"-class Macros
//@{

/// Utility macro for declaring standard Asset-class variables and functions.
///
/// @param[in] TYPE    Asset type.
/// @param[in] PARENT  Parent object type.
#define HELIUM_DECLARE_ASSET( TYPE, PARENT ) \
		HELIUM_DECLARE_CLASS_NO_REGISTRAR( TYPE, PARENT ) \
	public: \
		virtual const Helium::AssetType* GetAssetType() const; \
		virtual size_t GetInstanceSize() const; \
		virtual Helium::Asset* InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const; \
		virtual void InPlaceDestroy(); \
		static const Helium::AssetType* InitStaticType(); \
		static void ReleaseStaticType(); \
		static const Helium::AssetType* GetStaticType();\
		static Helium::AssetRegistrar< TYPE, TYPE::Base > s_Registrar;

/// Utility macro for implementing standard Asset-class variables and functions, without implementing
/// InitStaticType().
///
/// @param[in] TYPE    Asset type.
/// @param[in] MODULE  Module to which the type belongs.
#define HELIUM_IMPLEMENT_ASSET_NOINITTYPE( TYPE, MODULE ) \
	HELIUM_DEFINE_CLASS_NO_REGISTRAR( TYPE ) \
	\
	const Helium::AssetType* TYPE::GetAssetType() const \
	{ \
		return TYPE::GetStaticType(); \
	} \
	\
	size_t TYPE::GetInstanceSize() const \
	{ \
		return sizeof( *this ); \
	} \
	\
	Helium::Asset* TYPE::InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const \
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
		this->~This(); \
	} \
	\
	void TYPE::ReleaseStaticType() \
	{ \
		if( s_MetaClass ) \
		{ \
			Helium::AssetType::Unregister( static_cast< const Helium::AssetType* >( s_MetaClass->m_Tag ) ); \
			s_MetaClass = NULL; \
		} \
	} \
	\
	const Helium::AssetType* TYPE::GetStaticType() \
	{ \
		HELIUM_ASSERT( s_MetaClass ); \
		return static_cast< const Helium::AssetType* >( s_MetaClass->m_Tag ); \
	}\
	Helium::AssetRegistrar< TYPE, TYPE::Base > TYPE::s_Registrar( TXT( #TYPE ) );

/// Utility macro for implementing standard Asset-class variables and functions.
///
/// @param[in] TYPE        Asset type.
/// @param[in] MODULE      Module to which the type belongs.
/// @param[in] TYPE_FLAGS  Type flags.
#define HELIUM_IMPLEMENT_ASSET( TYPE, MODULE, TYPE_FLAGS ) \
	HELIUM_IMPLEMENT_ASSET_NOINITTYPE( TYPE, MODULE ) \
	\
	const Helium::AssetType* TYPE::InitStaticType() \
	{ \
		HELIUM_ASSERT( s_MetaClass ); \
		if ( !s_MetaClass->m_Tag ) \
		{ \
			Helium::Package* pTypePackage = AssetType::GetTypePackage(); \
			HELIUM_ASSERT( pTypePackage ); \
			\
			const Helium::AssetType* pParentType = Base::InitStaticType(); \
			HELIUM_ASSERT( pParentType ); \
			\
			Helium::StrongPtr< TYPE > spTemplate = Helium::Reflect::AssertCast< TYPE >( s_MetaClass->m_Default ); \
			HELIUM_ASSERT( spTemplate ); \
			\
			Helium::AssetType::Create( \
				Reflect::GetMetaClass< TYPE >(), \
				pTypePackage, \
				pParentType, \
				spTemplate, \
				TYPE_FLAGS ); \
		} \
		\
		return static_cast< const Helium::AssetType* >( s_MetaClass->m_Tag ); \
	}

//@}

namespace Helium
{
	template< class ClassT, class BaseT >
	class AssetRegistrar : public Reflect::MetaClassRegistrar<ClassT, BaseT>
	{
	public:
		AssetRegistrar( const char* name );

		virtual void Register();
		virtual void Unregister();
	};

	template< class ClassT >
	class AssetRegistrar< ClassT, void > : public Reflect::MetaClassRegistrar<ClassT, Reflect::Object>
	{
	public:
		AssetRegistrar( const char* name );

		virtual void Register();
		virtual void Unregister();
	};

	class AssetType;
	typedef SmartPtr< AssetType > AssetTypePtr;

	class Asset;
	typedef Helium::StrongPtr< Asset > AssetPtr;
	typedef Helium::StrongPtr< const Asset > ConstAssetPtr;

	class Asset;
	typedef Helium::WeakPtr< Asset > AssetWPtr;
	typedef Helium::WeakPtr< const Asset > ConstAssetWPtr;

	class Package;
	typedef Helium::StrongPtr< Package > PackagePtr;
	typedef Helium::StrongPtr< const Package > ConstPackagePtr;

	class PackageLoader;

	class HELIUM_ENGINE_API AssetAwareThreadSynchronizer : public Helium::NonCopyable
	{
	public:
		AssetAwareThreadSynchronizer();
		~AssetAwareThreadSynchronizer();

		void Sync();

		static void AcquireHardAssetLock();
		static void ReleaseHardAssetLock();

		typedef StaticScopeLock< AssetAwareThreadSynchronizer::AcquireHardAssetLock, AssetAwareThreadSynchronizer::ReleaseHardAssetLock > Lock;

	private:
		friend Asset;

		static void IncrementLockedThreadCount();
		static void DecrementLockedThreadCount();

		static volatile int32_t m_TotalThreadCount;
		static volatile int32_t m_LockedThreadCount;

		static ReadWriteLock m_AssetSyncLock;
		static Condition m_ThreadIsStalledCondition;
		static volatile bool m_BlockThreads;
	};

	/// Base class for the engine's game object system.
	class HELIUM_ENGINE_API Asset : public Helium::Reflect::Object
	{
	public:
		HELIUM_DECLARE_CLASS_NO_REGISTRAR( Asset, Reflect::Object );

		/// Destruction callback type.
		typedef void ( CUSTOM_DESTROY_CALLBACK )( Asset* pObject );

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
			FLAG_PACKAGE          = 1 << 8,

			/// Asset has been modified since loaded
			FLAG_CHANGED_SINCE_LOADED = 1 << 9,
			/// Asset is being kept in memory forcibly (for edit time support). TODO: Do we need this?
			FLAG_EDITOR_FORCIBLY_LOADED = 1 << 10,
			/// When an asset is loaded, we fire a loaded event only once (even if requested on multiple threads). This
			/// flag ensures the event gets fired exactly once
			FLAG_LOAD_EVENT_FIRED = 1 << 11
		};

		/// Object rename parameters.
		struct HELIUM_ENGINE_API RenameParameters
		{
			/// Object name.
			Name name;
			/// Owner.
			AssetPtr spOwner;
			/// Instance index (invalid index value for no instance index, INSTANCE_INDEX_AUTO to auto-assign).
			uint32_t instanceIndex;

			/// @name Construction/Destruction
			//@{
			inline RenameParameters();
			//@}
		};

		/// @name Construction/Destruction
		//@{
		Asset();
		virtual ~Asset();
		//@}

		/// @name Asset Interface
		//@{
		inline Name GetName() const;
		inline Asset* GetOwner() const;
		inline Package* GetOwningPackage();
		inline uint32_t GetInstanceIndex() const;
		bool Rename( const RenameParameters& rParameters );
		
		// Override for Object::Clone
		virtual Reflect::ObjectPtr Clone();
		virtual bool CloneAsset(AssetPtr _asset_ptr);

		inline uint32_t GetId() const;

		inline uint32_t GetFlags() const;
		inline bool GetAnyFlagSet( uint32_t flagMask ) const;
		inline bool GetAllFlagsSet( uint32_t flagMask ) const;
		uint32_t SetFlags( uint32_t flagMask );
		uint32_t ClearFlags( uint32_t flagMask );
		uint32_t ToggleFlags( uint32_t flagMask );

		virtual Reflect::ObjectPtr GetTemplate() const HELIUM_OVERRIDE;
		AssetPtr GetTemplateAsset() const;

		inline const AssetWPtr& GetFirstChild() const;
		inline const AssetWPtr& GetNextSibling() const;
		Asset* FindChild( Name name, uint32_t instanceIndex = Invalid< uint32_t >() ) const;

		inline AssetPath GetPath() const;

		inline bool IsFullyLoaded() const;
		inline bool IsDefaultTemplate() const;
		inline bool IsPackage() const;

		virtual void RefCountPreDestroy();
		virtual void RefCountDestroy();  // This should only be called by the reference counting system!
		//@}

		/// @name RTTI
		//@{
		virtual const AssetType* GetAssetType() const;
		inline bool IsInstanceOf( const AssetType* pType ) const;
		//@}

		/// @name Serialization
		//@{

		static void PopulateMetaType( Reflect::MetaStruct& comp);

		virtual bool NeedsPrecacheResourceData() const;
		virtual bool BeginPrecacheResourceData();
		virtual bool TryFinishPrecacheResourceData();

		virtual void FinalizeLoad();
		inline void ConditionalFinalizeLoad();

#if HELIUM_TOOLS
		virtual void PostSave();
		Asset *GetSourceAsset();
		const FilePath *GetAssetFileSystemPath();
		uint64_t GetAssetFileTimeStamp();

		bool SaveAsset();
#endif

		bool IsTransient() const;
		//@}

		/// @name Creation Utility Functions
		//@{
		virtual size_t GetInstanceSize() const;
		virtual Asset* InPlaceConstruct( void* pMemory, CUSTOM_DESTROY_CALLBACK* pDestroyCallback ) const;
		virtual void InPlaceDestroy();
		//@}

		/// @name Asset Management
		//@{
		static bool CreateObject(
			AssetPtr& rspObject, const AssetType* pType, Name name, Asset* pOwner,
			Asset* pTemplate = NULL, bool bAssignInstanceIndex = false );
		template< typename T > static bool Create(
			StrongPtr< T >& rspObject, Name name, Asset* pOwner, T* pTemplate = NULL,
			bool bAssignInstanceIndex = false );

		static Asset* FindObject( AssetPath path );
		template< typename T > static T* Find( AssetPath path );

		static Asset* FindChildOf( const Asset* pObject, Name name, uint32_t instanceIndex = Invalid< uint32_t >() );
		static Asset* FindChildOf(
			const Asset* pObject, const Name* pRelativePathNames, const uint32_t* pInstanceIndices, size_t nameDepth,
			size_t packageDepth );

		static bool RegisterObject( Asset* pObject );
		static void UnregisterObject( Asset* pObject );

		static void ReplaceAsset( Asset* pNewAsset, const AssetPath &objectToReplace );

		static void Shutdown();
		//@}

		/// @name Static Interface
		//@{
		static const AssetType* InitStaticType();
		static void ReleaseStaticType();
		static const AssetType* GetStaticType();
		//@}

		static Asset *GetFirstTopLevelAsset() { return sm_wpFirstTopLevelObject.Get(); }
		
		static AssetRegistrar< Asset, void > s_Registrar;

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
		typedef ConcurrentHashMap< AssetPath, NameInstanceIndexMap > ChildNameInstanceIndexMap;

		/// Object name.
		Name m_name;
		/// Instance index.
		uint32_t m_instanceIndex;
		/// Object ID.
		uint32_t m_id;
		/// Object flags.
		volatile uint32_t m_flags;
		/// Override object template (null if using the type's default object).
		AssetPtr m_spTemplate;

		/// Object owner.
		AssetPtr m_spOwner;
		/// First child object.
		AssetWPtr m_wpFirstChild;
		/// Next sibling object.
		AssetWPtr m_wpNextSibling;

		/// Full object path name.
		AssetPath m_path;

		/// Custom callback for notifying that this object should be destroyed when its reference count drops to zero
		/// (provided for custom object allocation schemes).
		CUSTOM_DESTROY_CALLBACK* m_pCustomDestroyCallback;

		/// Global object list.
		static SparseArray< AssetWPtr > sm_objects;
		/// First object in the list of top-level objects.
		static AssetWPtr sm_wpFirstTopLevelObject;

		/// Name instance lookup.
		static ChildNameInstanceIndexMap* sm_pNameInstanceIndexMap;
		/// Empty object name instance map.
		static Pair< AssetPath, NameInstanceIndexMap >* sm_pEmptyNameInstanceIndexMap;
		/// Empty name instance index lookup set.
		static Pair< Name, InstanceIndexSet >* sm_pEmptyInstanceIndexSet;

		/// Read-write lock for synchronizing access to the object lists.
		static ReadWriteLock sm_objectListLock;

		/// Cached serialization buffer.
		static DynamicArray< uint8_t > sm_serializationBuffer;

		/// @name Private Utility Functions
		//@{
		void UpdatePath();
		//@}

		/// @name Reference Counting Support, Private
		//@{
		static void StandardCustomDestroy( Asset* pObject );
		//@}

		/// @name Static Asset Management
		//@{
		static ChildNameInstanceIndexMap& GetNameInstanceIndexMap();
		//@}
	};

	/// @defgroup objectcast Type-checking Asset Casting Functions
	//@{
	template< typename TargetType, typename SourceType > TargetType* DynamicCast( SourceType* pObject );
	template< typename TargetType, typename SourceType > TargetType* StaticCast( SourceType* pObject );
	//@}

	/// Run-time type information for Asset classes.
	class HELIUM_ENGINE_API AssetType : public Helium::AtomicRefCountBase< AssetType >
	{
	public:
		/// Type lookup hash map.
		typedef HashMap< Name, AssetTypePtr > LookupMap;

		/// General type flags.
		enum EFlag
		{
			/// Type should not be instantiated.
			FLAG_ABSTRACT    = 1 << 0,
			/// Instances of this type should never be saved or loaded in a package.
			FLAG_TRANSIENT   = 1 << 1,
			/// Instances of this type cannot be used as templates.
			FLAG_NO_TEMPLATE = 1 << 2,
			/// Use the parent's asset file timestamp as our own (useful for example 
			/// with shaders which generate shader variants based on the shader's .hlsl file)
			FLAG_GENERATED_FROM_OWNER = 1 << 3
		};

		/// Type iterator.
		class HELIUM_ENGINE_API ConstIterator
		{
			friend class AssetType;

		public:
			/// @name Construction/Destruction
			//@{
			inline ConstIterator();
			//@}

			/// @name Overloaded Operators
			//@{
			inline const AssetType& operator*() const;
			inline const AssetType* operator->() const;

			inline ConstIterator& operator++();
			inline ConstIterator operator++( int );
			inline ConstIterator& operator--();
			inline ConstIterator operator--( int );

			inline bool operator==( const ConstIterator& rOther ) const;
			inline bool operator!=( const ConstIterator& rOther ) const;
			inline bool operator<( const ConstIterator& rOther ) const;
			inline bool operator>( const ConstIterator& rOther ) const;
			inline bool operator<=( const ConstIterator& rOther ) const;
			inline bool operator>=( const ConstIterator& rOther ) const;
			//@}

		private:
			/// Type map iterator.
			LookupMap::ConstIterator m_iterator;

			/// @name Construction/Destruction, Private
			//@{
			inline explicit ConstIterator( LookupMap::ConstIterator iterator );
			//@}
		};

		/// @name Construction/Destruction
		//@{
		AssetType();
		virtual ~AssetType();
		//@}

		/// @name Data Access
		//@{
		inline Name GetName() const;
		inline const Reflect::MetaClass* GetMetaClass() const;
		inline const AssetType* GetBaseType() const;
		Asset* GetTemplate() const;

		inline uint32_t GetFlags() const;
		//@}

		/// @name Static Type Registration
		//@{
		inline static Package* GetTypePackage();
		static void SetTypePackage( Package* pPackage );

		static AssetType* Create( const Reflect::MetaClass* pClass, Package* pTypePackage, const AssetType* pParent, Asset* pTemplate, uint32_t flags );
		static void Unregister( const AssetType* pType );

		static AssetType* Find( Name typeName );

		static ConstIterator GetTypeBegin();
		static ConstIterator GetTypeEnd();

		static void Shutdown();
		//@}

	private:
		/// Reflection class information.
		const Reflect::MetaClass* m_class;
		/// Name table entry for this type.
		Name m_name;
		/// Type flags.
		uint32_t m_flags;

		/// Main package containing all template objects.
		static PackagePtr sm_spTypePackage;
		/// Type lookup hash map instance.
		static LookupMap* sm_pLookupMap;
	};

	/// Asset package.
	class HELIUM_ENGINE_API Package : public Asset
	{
		HELIUM_DECLARE_ASSET( Package, Asset );

	public:
		/// @name Construction/Destruction
		//@{
		Package();
		virtual ~Package();
		//@}

		/// @name Loader Information
		//@{
		inline PackageLoader* GetLoader() const;
		void SetLoader( PackageLoader* pLoader );
		//@}

		/// @name Package Serialization
		//@{
		void SavePackage();
		//@}

	private:
		/// Package loader.
		PackageLoader* m_pLoader;
	};

	typedef Helium::StrongPtr< Package > PackagePtr;
	typedef Helium::StrongPtr< const Package > ConstPackagePtr;

}

#include "Engine/Asset.inl"
