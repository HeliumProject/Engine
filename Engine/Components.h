#pragma once

#include <vector>

#include "Engine/Asset.h"
#include "Reflect/Structure.h"
#include "Foundation/Map.h"
#include "Foundation/SmartPtr.h"
#include "Engine/Engine.h"


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
		const static uint32_t POOL_ALIGN_SIZE = 32;
		const static uint32_t POOL_ALIGN_SIZE_MASK = ~(POOL_ALIGN_SIZE-1);
		
#if HELIUM_HEAP
		HELIUM_ENGINE_API extern Helium::DynamicMemoryHeap g_ComponentAllocator;
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
			virtual uintptr_t GetOffsetOfComponent() const
			{
				// So this is awful. Base component is not virtual but downstream components *might* be.
				// You might do this exact code with a null pointer, but it won't work because null pointers
				// do not get pointer fixup with a static_cast.
				T *t = reinterpret_cast< T* >( 0x80000000 );
				Component *c = static_cast<Component *>(t);
				return reinterpret_cast<uintptr_t>( c ) - 0x80000000;
			}
		};

		struct IHasComponents
		{
			// Called "Virtual" to discourage using these functions.. usually you can call the non-virtual implementation
			virtual World *VirtualGetWorld() = 0;
			virtual ComponentCollection &VirtualGetComponents() = 0;
		};
		
		template< class ClassT, class BaseT >
		struct ComponentRegistrar : public Reflect::StructureRegistrar< ClassT, BaseT >
		{
		public:
			ComponentRegistrar(const tchar_t* name, ComponentIndex _count);
			virtual void Register();

			ComponentIndex m_Count;
		};

		template< class ClassT >
		struct ComponentRegistrar< ClassT, void > : public Reflect::StructureRegistrar< ClassT, void >
		{
		public:
			ComponentRegistrar(const tchar_t* name);
			virtual void Register();
		};
		
		struct HELIUM_ENGINE_API DataInline
		{
			void*            m_Owner;
			uint16_t         m_OffsetToPoolStart;
			ComponentIndex   m_Next;
			ComponentIndex   m_Previous;
			GenerationIndex  m_Generation;
			bool             m_Delete;
		};
		
		struct HELIUM_ENGINE_API DataParallel
		{
			ComponentCollection*  m_Collection;
			ComponentIndex        m_RosterIndex;
		};
		
		struct HELIUM_ENGINE_API Pool
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

			Component*                 Allocate(void *owner, ComponentCollection &collection);
			void                       Free(Component *component);
			void                       InsertIntoChain(Component *_insertee, ComponentIndex _insertee_index, Component *nextComponent);
			void                       RemoveFromChain(Component *_component, ComponentIndex index);

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
		
		HELIUM_ENGINE_API void                Initialize();
		HELIUM_ENGINE_API void                Cleanup();
		HELIUM_ENGINE_API void                Tick();
		
		HELIUM_ENGINE_API TypeId              RegisterType(
			const Reflect::Structure *_structure, 
			TypeData&                 _type_data, 
			TypeData*                 _base_type_data, 
			uint16_t                  _count);
		HELIUM_ENGINE_API const TypeData*     GetTypeData( TypeId type );

		HELIUM_ENGINE_API ComponentManager*   CreateManager( World *pWorld );

        template <class T>  TypeId GetType();
	}

	class HELIUM_ENGINE_API ComponentManager
	{
	public:
		virtual                    ~ComponentManager();

		void                       Tick();
		void                       RegisterComponentPtr( ComponentPtrBase &pPtr );
		inline World *             GetWorld() const;

		inline Component*          Allocate(Components::TypeId type, void *pOwner, ComponentCollection &rCollection);

		inline size_t              CountAllocatedComponents( Components::TypeId typeId ) const;
		inline size_t              CountAllocatedComponentsThatImplement( Components::TypeId typeId ) const;

		//inline const ComponentList GetAllocatedComponents( Components::TypeId typeId ) const;
		//inline void                GetAllocatedComponentsThatImplement( Components::TypeId typeId, DynamicArray< ComponentList > &lists ) const;

		template < class T > T*    Allocate( void *pOwner, ComponentCollection &rCollection )
		{
			return static_cast< T* >( Allocate( Components::GetType<T>(), pOwner, rCollection ) );
		}
		
		template < class T > size_t    CountAllocatedComponents()
		{
			return CountAllocatedComponents( Components::GetType<T>() );
		}
		
		template < class T > size_t    CountAllocatedComponentsThatImplement()
		{
			return CountAllocatedComponentsThatImplement( Components::GetType<T>() );
		}

		inline const Components::Pool *GetPool( Components::TypeId typeId )
		{
			return m_Pools[ typeId ];
		}
		
		//template < class T > const Components::ComponentListT<T>    GetAllocatedComponents()
		//{
		//	return *reinterpret_cast< const Components::ComponentListT<T> *>( 
		//		&GetAllocatedComponents( Components::GetType<T>() ) 
		//		);
		//}

		//template < class T > void   GetAllocatedComponentsThatImplement( DynamicArray< Components::ComponentListT<T> > &lists )
		//{
		//	GetAllocatedComponentsThatImplement( 
		//		Components::GetType<T>(), 
		//		*reinterpret_cast< DynamicArray< ComponentList > *>(&lists) 
		//		);
		//}

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
		inline void  Advance();
		inline void  ResetToBeginning();

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
		inline ComponentIteratorT( ComponentManager &rManager )
			: ComponentIteratorBaseT( rManager )
		{
			m_Types = &m_OwnedTypes;
			ResetToBeginning();
		}

	private:
		DynamicArray< Components::TypeId > m_OwnedTypes;
	};

	template <class T>
	class ImplementingComponentIterator : public ComponentIteratorBaseT< T >
	{
	public:
		inline ImplementingComponentIterator( ComponentManager &rManager )
			: ComponentIteratorBaseT( rManager )
		{
			m_Types = &Components::GetTypeData( Components::GetType<T>() )->m_ImplementingTypes;
			ResetToBeginning();
		}
	};
	
	class HELIUM_ENGINE_API ComponentCollection
	{
	public:
		inline ComponentCollection();
		inline ~ComponentCollection();

		inline Component *GetFirst( Components::TypeId type );
		inline void       GetAll( Components::TypeId type, DynamicArray<Component *> &m_Components );
		inline void       GetAllThatImplement( Components::TypeId type, DynamicArray<Component *> &m_Components );
		inline void       ReleaseAll();
		
		template <class T> inline T *GetFirst() { return static_cast<T *>( GetFirst( Components::GetType<T>() ) ); }

	private:
		friend Components::Pool;
		Map< Components::TypeId, Component * > m_Components;
	};

	//! All components have some data for bookkeeping
	class HELIUM_ENGINE_API Component
	{
	public:
		HELIUM_DECLARE_BASE_COMPONENT( Helium::Component )
		static void PopulateStructure( Reflect::Structure& comp ) { }

		inline ComponentManager*             GetComponentManager() const;
		inline ComponentCollection*          GetComponentCollection() const;
		inline void*                         GetOwner() const;
		inline World*                        GetWorld() const;
		inline void                          FreeComponent();
		inline void                          FreeComponentDeferred();

		inline const Components::DataInline& GetInlineData() const;

		template <class T> T* AllocateSiblingComponent() { return GetComponentManager()->Allocate<MeshSceneObjectTransform>( GetOwner(), *GetComponentCollection() ); }
		
		static Components::ComponentRegistrar<Component, void> s_ComponentRegistrar;

	private:
		friend Components::Pool;
		friend ComponentPtrBase;
		Components::DataInline m_InlineData;
	};

	
    // Code that need not be template aware goes here
    class HELIUM_ENGINE_API ComponentPtrBase
    {
    public:
        inline void Check() const;
        inline bool IsGood() const;
        inline void Set(Component *_component);

        ComponentPtrBase *GetNextComponetPtr() { return m_Next; }
        friend ComponentManager;
		friend void Helium::Components::Tick();

    protected:
        inline ComponentPtrBase();
        inline ~ComponentPtrBase();

        inline void Set(Component *_component) const;
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

#include "Engine/Components.inl"
