#pragma once

#include <vector>

#include "Reflect/Structure.h"
#include "Reflect/Registry.h"
#include "Reflect/Object.h"
#include "Foundation/Map.h"
#include "Foundation/SmartPtr.h"
#include "Framework/Framework.h"


#define _COMPONENT_BOILERPLATE(__Type)                        \
	public:                                                             \
	/* Every class implementing Component must have this function */  \
	static Helium::Components::TypeData &GetStaticComponentTypeData()      \
		{                                                                 \
		static Helium::Components::TypeDataT<__Type> data;                      \
		return data;                                                    \
		}                                                                 \
		\
\
	__Type *GetNextComponent() const\
	{\
	  return static_cast<__Type *>(Helium::Components::Pool::GetPool( this )->GetComponent( GetInlineData().m_Next ) );\
	}


		//Internal use only
#define HELIUM_DECLARE_BASE_COMPONENT( __Type )                         \
	REFLECT_DECLARE_BASE_STRUCTURE(__Type)                       \
	_COMPONENT_BOILERPLATE(__Type)

#define _HELIUM_DECLARE_COMPONENT_REGISTRAR( __Type, __Base ) \
	static Helium::Components::ComponentRegistrar<__Type, __Base> s_ComponentRegistrar; \
	typedef __Base ComponentBase;

		//! Add to any component that will not be instantiated
#define HELIUM_DECLARE_ABSTRACT_COMPONENT( __Type, __Base )         \
	REFLECT_DECLARE_DERIVED_STRUCTURE(__Type, __Base)                          \
	_COMPONENT_BOILERPLATE(__Type)                                 \
	_HELIUM_DECLARE_COMPONENT_REGISTRAR(__Type, __Base)

		//! Add to any component that will be instantiated
#define HELIUM_DECLARE_COMPONENT( __Type, __Base )                  \
	REFLECT_DECLARE_DERIVED_STRUCTURE(__Type, __Base)                             \
	_COMPONENT_BOILERPLATE(__Type)                                 \
	_HELIUM_DECLARE_COMPONENT_REGISTRAR(__Type, __Base)

#define HELIUM_DEFINE_COMPONENT( __Type, __Count ) \
	Helium::Components::ComponentRegistrar<__Type, __Type::ComponentBase> __Type::s_ComponentRegistrar(#__Type, __Count); \
	REFLECT_DEFINE_DERIVED_STRUCTURE( __Type )

#define HELIUM_DEFINE_ABSTRACT_COMPONENT( __Type, __Count ) \
	Helium::Components::ComponentRegistrar<__Type, __Type::ComponentBase> __Type::s_ComponentRegistrar(#__Type, __Count); \
	REFLECT_DEFINE_DERIVED_STRUCTURE( __Type )

#define HELIUM_COMPONENT_PTR_CHECK_FREQUENCY (256)
#define HELIUM_COMPONENT_POOL_ALIGN_SIZE (32)
#define HELIUM_COMPONENT_POOL_ALIGN_SIZE_MASK (~(POOL_ALIGN_SIZE-1))

namespace Helium
{
	class ComponentManager;
	class ComponentCollection;
	class Component;
	class World;
	class ComponentPtrBase;

	namespace Components
	{
		template <class T>
		struct ComponentListT
		{
			T* const * m_pComponents;
			size_t m_Count;
		};

	}
	
	typedef Components::ComponentListT< Component > ComponentList;

	typedef Helium::AutoPtr< ComponentManager > ComponentManagerPtr;

	namespace Components
	{
		//! Component type id (not the same as the reflect class id).
		typedef uint16_t TypeId;
		typedef uint16_t ComponentIndex;
		typedef uint16_t ComponentSizeType;
		typedef uint8_t GenerationIndex;

		const static uint32_t COMPONENT_PTR_CHECK_FREQUENCY = 256;
		const static uintptr_t POOL_ALIGN_SIZE = 32;
		const static uintptr_t POOL_ALIGN_SIZE_MASK = ~(POOL_ALIGN_SIZE-1);
		
#if HELIUM_HEAP
		HELIUM_FRAMEWORK_API extern Helium::DynamicMemoryHeap g_ComponentAllocator;
#else
		static Helium::DefaultAllocator g_ComponentAllocator;
#endif

		struct TypeData
		{
			inline TypeData();
			TypeId m_TypeId;
			
			const Reflect::Structure*  m_Structure;
			DynamicArray<TypeId>       m_ImplementedTypes;       //< Parent type IDs of this type
			DynamicArray<TypeId>       m_ImplementingTypes;      //< Child types IDs of this type
			ComponentIndex             m_DefaultCount;           //< Default number of components of this type to make

			virtual void       Construct(Component *ptr) const = 0;
			virtual void       Destruct(Component *ptr) const = 0;
			virtual uintptr_t  GetOffsetOfComponent() const = 0;

			inline ComponentSizeType  GetSize() const;
		};

		template <class T>
		struct TypeDataT : public TypeData
		{
			virtual void      Construct(Component *ptr) const;
			virtual void      Destruct(Component *ptr) const;
			virtual uintptr_t GetOffsetOfComponent() const;
		};

		struct IHasComponents
		{
			// Called "Virtual" to discourage using these functions.. usually you can call the non-virtual implementation
			virtual ComponentManager* VirtualGetComponentManager() = 0;
			virtual ComponentCollection& VirtualGetComponents() = 0;
		};
		
		template< class ClassT, class BaseT >
		struct ComponentRegistrar : public Reflect::StructureRegistrar< ClassT, BaseT >
		{
		public:
			ComponentRegistrar(const char* name, ComponentIndex _count);
			virtual void Register();

			ComponentIndex m_Count;
		};

		template< class ClassT >
		struct ComponentRegistrar< ClassT, void > : public Reflect::StructureRegistrar< ClassT, void >
		{
		public:
			ComponentRegistrar(const char* name);
			virtual void Register();
		};
		
		struct HELIUM_FRAMEWORK_API DataInline
		{
			IHasComponents*  m_Owner;
			uint16_t         m_OffsetToPoolStart;
			ComponentIndex   m_Next;
			ComponentIndex   m_Previous;
			GenerationIndex  m_Generation;
			bool             m_Delete;
		};
		
		struct HELIUM_FRAMEWORK_API DataParallel
		{
			ComponentCollection*  m_Collection;
			ComponentIndex        m_RosterIndex;
		};
		
		struct HELIUM_FRAMEWORK_API Pool
		{
		public:
			static Pool*               CreatePool( ComponentManager *pComponentManager, const TypeData &rTypeData, ComponentIndex count );
			static void                DestroyPool( Pool *pPool );
			static inline Pool*        GetPool( const Component *component );
									   
			inline TypeId              GetTypeId() const;
			inline ComponentManager*   GetComponentManager() const;
			inline World*              GetWorld() const;
			inline Component*          GetComponent(ComponentIndex index) const;
			inline ComponentIndex      GetComponentIndex(const Component *component) const;
			inline ComponentCollection* GetComponentCollection(const Component *component) const;

			inline void*               GetComponentOwner(const Component *component) const;
									   
			inline Component*          GetNext(Component *component) const;
			inline Component*          GetNext(ComponentIndex index) const;
			inline ComponentIndex      GetNextIndex(Component *component) const;
			inline ComponentIndex      GetNextIndex(ComponentIndex index) const;
			inline Component*          GetPrevious(Component *component) const;
			inline Component*          GetPrevious(ComponentIndex index) const;
			inline ComponentIndex      GetPreviousIndex(Component *component) const;
			inline ComponentIndex      GetPreviousIndex(ComponentIndex index) const;
			inline GenerationIndex     GetGeneration(ComponentIndex index) const;
			inline ComponentIndex      GetAllocatedCount() const;
			inline Component * const * GetAllocatedComponents() const;
			inline Component *         GetComponentByRosterIndex(ComponentIndex index) const;

			Component*                 Allocate(Components::IHasComponents *owner, ComponentCollection &collection);
			void                       Free(Component *component);
			void                       InsertIntoChain(Component *_insertee, ComponentIndex _insertee_index, Component *nextComponent);
			void                       RemoveFromChain(Component *_component, ComponentIndex index);

#if HELIUM_TOOLS
			void SpewRosterToTty();
#endif

		private:

			inline uintptr_t           GetFirstComponentPtr() const;
									   
			DynamicArray<Component *>  m_Roster;
			DataParallel*              m_ParallelData;
			World*                     m_World;
			ComponentManager*          m_ComponentManager;
			const TypeData*            m_Type;
			uintptr_t                  m_ComponentOffset;
			TypeId                     m_TypeId;
			ComponentSizeType          m_ComponentSize;
			ComponentIndex             m_FirstUnallocatedIndex;
		};
		
		HELIUM_FRAMEWORK_API void                Initialize();
		HELIUM_FRAMEWORK_API void                Cleanup();
		HELIUM_FRAMEWORK_API void                Tick();
		
		HELIUM_FRAMEWORK_API TypeId              RegisterType(
			const Reflect::Structure *_structure, 
			TypeData&                 _type_data, 
			TypeData*                 _base_type_data, 
			uint16_t                  _count);
		HELIUM_FRAMEWORK_API const TypeData*     GetTypeData( TypeId type );

		HELIUM_FRAMEWORK_API ComponentManager*   CreateManager( World *pWorld );

		template <class T>  TypeId GetType();
	}

	class HELIUM_FRAMEWORK_API ComponentManager
	{
	public:
		virtual                  ~ComponentManager();

		void                     RegisterComponentPtr( ComponentPtrBase &pPtr );
		inline World*            GetWorld() const;
		inline const Components::Pool*  GetPool( Components::TypeId typeId );

		inline Component*        Allocate(Components::TypeId type, Components::IHasComponents *pOwner, ComponentCollection &rCollection);
		inline size_t            CountAllocatedComponents( Components::TypeId typeId ) const;
		size_t                   CountAllocatedComponentsThatImplement( Components::TypeId typeId ) const;

		template < class T > T*        Allocate( Components::IHasComponents *pOwner, ComponentCollection &rCollection );
		template < class T > size_t    CountAllocatedComponents();
		template < class T > size_t    CountAllocatedComponentsThatImplement();

	private:
		friend ComponentManager* Helium::Components::CreateManager( World *pWorld );
		ComponentManager(World *pWorld);

		World *m_World;
		DynamicArray<Components::Pool *> m_Pools;
	};


	class ComponentIteratorBase
	{
	public:
		inline ComponentIteratorBase(ComponentManager &rManager, const DynamicArray<Components::TypeId> &types);

		inline Component *GetBaseComponent();
		inline void       Advance();
		inline void       ResetToBeginning();

	protected:
		inline ComponentIteratorBase(ComponentManager &rManager);
		const DynamicArray<Components::TypeId> *m_Types;

	private:
		
		inline void  SkipToNextType();

		DynamicArray<Components::TypeId>::ConstIterator m_TypesIterator;
		ComponentManager &m_Manager;
		const Components::Pool *m_pPool;
		Component *m_pComponent;
		Components::ComponentIndex m_Index;
	};

	template <class T>
	class ComponentIteratorBaseT : public ComponentIteratorBase
	{
	public:
		inline T *   operator*();
		inline T *   operator->();

	protected:
		inline ComponentIteratorBaseT( ComponentManager &rManager );
	};

	template <class T>
	class ComponentIteratorT : public ComponentIteratorBaseT< T >
	{
	public:
		inline ComponentIteratorT( ComponentManager &rManager );

	private:
		DynamicArray< Components::TypeId > m_OwnedTypes;
	};

	template <class T>
	class ImplementingComponentIterator : public ComponentIteratorBaseT< T >
	{
	public:
		inline ImplementingComponentIterator( ComponentManager &rManager );
	};
	
	class HELIUM_FRAMEWORK_API ComponentCollection
	{
	public:
		inline ComponentCollection();
		inline ~ComponentCollection();

		inline Component *GetFirst( Components::TypeId type );
		inline void       GetAll( Components::TypeId type, DynamicArray<Component *> &m_Components );
		inline void       GetAllThatImplement( Components::TypeId type, DynamicArray<Component *> &m_Components );
		inline void       ReleaseEach( Components::TypeId type );
		inline void       ReleaseAll();

		template <class T> inline T *GetFirst() { return static_cast<T *>( GetFirst( Components::GetType<T>() ) ); }
		template <class T> void      ReleaseEach() { ReleaseEach( Components::GetType<T>() ); }

#if HELIUM_TOOLS
		void SpewToTty();
#endif

	private:
		friend Components::Pool;
		Map< Components::TypeId, Component * > m_Components;
	};

	//! All components have some data for bookkeeping
	class HELIUM_FRAMEWORK_API Component
	{
	public:
		HELIUM_DECLARE_BASE_COMPONENT( Helium::Component )
		static void PopulateStructure( Reflect::Structure& comp ) { }

		inline ComponentManager*             GetComponentManager() const;
		inline ComponentCollection*          GetComponentCollection() const;
		inline Components::IHasComponents*   GetOwner() const;
		inline World*                        GetWorld() const;
		inline void                          FreeComponent();
		inline void                          FreeComponentDeferred();

		inline const Components::DataInline& GetInlineData() const;

		template <class T> T* AllocateSiblingComponent();
		
		static Components::ComponentRegistrar<Component, void> s_ComponentRegistrar;

	private:
		friend Components::Pool;
		friend ComponentPtrBase;
		Components::DataInline m_InlineData;
	};

	
	// Code that need not be template aware goes here
	class HELIUM_FRAMEWORK_API ComponentPtrBase
	{
	public:
		inline void Check() const;
		inline bool IsGood() const;
		inline void Reset(Component *_component = 0);

		ComponentPtrBase *GetNextComponetPtr() { return m_Next; }
		friend ComponentManager;
		friend void Helium::Components::Tick();

	protected:
		inline ComponentPtrBase();
		inline ~ComponentPtrBase();

		inline void Reset(Component *_component) const;
		void Unlink() const;
			
		// Component we point to. NOTE: This will ALWAYS be a type T component because 
		// this class never sets m_Component to anything but NULL. Our non-base template
		// class is the only way to construct this class or assign a pointer
		mutable Component *m_Component; 

	private:
		// Doubly linked list lets us unsplice ourself easily from list of "allocated" ComponentPtrs
		mutable ComponentPtrBase *m_Next;
		mutable ComponentPtrBase *m_Previous;
			
		// We set this generation when a component is assigned
		mutable Components::GenerationIndex m_Generation;

		// If not assigned to Helium::Invalid<uint16_t>(), this node is head of
		// linked list and is pointed to by g_ComponentPtrRegistry. Destruction of this
		// Ptr will automatically fix that reference.
		mutable uint16_t m_ComponentPtrRegistryHeadIndex;
	};

	// Code that uses T goes here
	template <class T>
	class ComponentPtr : public Helium::ComponentPtrBase
	{
	public:
		ComponentPtr();
		explicit ComponentPtr(T *_component);
		ComponentPtr( const ComponentPtr& _rhs );

		void operator=(T *_component);

		// Only safe to call this if you know the component was not deallocated since Check() was called
		T *UncheckedGet() const;
		
		T *Get();
		const T *Get() const;

		T &operator*();
		T *operator->();

	private:
	};
}

#include "Framework/Components.inl"
