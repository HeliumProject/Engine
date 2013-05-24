//#pragma once
//
//#include <vector>
//
//#include "Engine/Asset.h"
//#include "Reflect/Structure.h"
//#include "Foundation/Map.h"
//#include "Foundation/SmartPtr.h"
//#include "Engine/Engine.h"
//
//// TODO: Put the #defines here
//
//
////TODO: Review compiled code to make sure that iterators, function objects, etc.
////      are getting inlined properly
//namespace Helium
//{
//    class Entity;
//    class World;
//
//    namespace Components
//    {
//        //! Component type id (not the same as the reflect class id).
//        typedef uint16_t TypeId;
//        const static TypeId NULL_TYPE_ID = 0xFFFF;
//
//        typedef uint8_t GenerationIndex;
//        const static uint32_t COMPONENT_PTR_CHECK_FREQUENCY = 256;
//
//        class Component;
//        class System;
//        class ComponentPtrBase;
//
//        namespace Private
//        {
//            //! A static member of every component type (instrumented via the DECLARE_COMPONENT macro)
//
//            /// TODO: Avoid this macro by using reflect a bit more directly like AssetClasses do
//            struct TypeData
//            {
//                TypeData() : m_TypeId(NULL_TYPE_ID) { }
//                TypeId m_TypeId;
//            };
//            
//            // Inserts component in front the given component in its chain
//            void          InsertIntoChain(Component *_insertee, Component *_next_component);
//
//            // Splices component out of the chain it is in
//            void          RemoveFromChain(Component *_component);
//        }
//
//        typedef std::vector<Component *> V_Components;
//        typedef Helium::Map<TypeId, Component *> M_Components;
//
//        //! Any object that can have components attached will own a ComponentSet
//        struct ComponentCollection
//        {
//            M_Components m_Components;
//        };
//
//        namespace Private
//        {
//            //! I want to generate a couple functions that require the T of the components the type contains.
//            struct IComponentTypeTCallbacks
//            {
//                virtual void CallConstructor(Component *_component) = 0;
//                virtual void DestroyComponents(struct ComponentType &_type_info) = 0;
//            };
//
//            //! Registering a component type creates one of these
//            struct ComponentType
//            {
//                const Reflect::Structure *m_Structure;              //< Class of the component's type
//                IComponentTypeTCallbacks *m_TCallbacks;             //< Pointer to template object that implements a couple useful functions.. it's a raw
//                std::vector<TypeId>       m_ImplementedTypes;       //< Parent type IDs of this type
//                std::vector<TypeId>       m_ImplementingTypes;      //< Child types IDs of this type
//                size_t                    m_InstanceSize;           //< Cache sizeof(Component Type).. to save frequent pointer resolve of m_Class
//                uint16_t                  m_FirstUnallocatedIndex;  //< Pointer to "free" pointer of roster
//
//                // parallel arrays
//                DynamicArray<uint16_t>    m_Roster;                 //< List of component indeces.. first all allocated then all deallocated components
//                void *                    m_Pool;               //< Pointer to the memory block that contains our component instances contiguously
//                //  pointer because auto ptrs can't be copied and this struct goes in a DynamicArray
//            };
//            typedef DynamicArray<ComponentType> A_ComponentTypes;
//
//			struct Component
//
//			HELIUM_ENGINE_API Helium::Components::Component *GetComponentFromIndex(TypeId _type, uint32_t _index);
//
//            template <class T>
//            struct ComponentTypeTCallbacks : public IComponentTypeTCallbacks
//            {
//                virtual void CallConstructor(Component *_component)
//                {
//                    //static_cast<T *>(_component)
//                    new (_component) T();
//                }
//
//                virtual void DestroyComponents(ComponentType &_type_info)
//                {
//                    HELIUM_DELETE_A(g_ComponentAllocator, static_cast<T *>(_type_info.m_Pool));
//                    Private::TypeData &data = T::GetStaticComponentTypeData();
//                    data.m_TypeId = NULL_TYPE_ID;
//                }
//            };
//
//            template< class ClassT, class BaseT >
//            struct ComponentRegistrar : public Reflect::StructureRegistrar< ClassT, BaseT >
//            {
//            public:
//                ComponentRegistrar(const tchar_t* name, uint16_t _count)
//                    : StructureRegistrar(name),
//                      m_Count(_count)
//                {
//
//                }
//
//                virtual void Register()
//                {                    
//                    if (ClassT::GetStaticComponentTypeData().m_TypeId == NULL_TYPE_ID)
//                    {
//                        BaseT::s_ComponentRegistrar.Register();
//                        StructureRegistrar::Register();
//                        ClassT *components_block = HELIUM_NEW_A(g_ComponentAllocator, ClassT, m_Count);
//                        TypeId type_id = RegisterType(
//                            Reflect::GetStructure< ClassT >(), 
//                            ClassT::GetStaticComponentTypeData(), 
//                            &BaseT::GetStaticComponentTypeData(), 
//                            m_Count, 
//                            components_block, 
//                            new ComponentTypeTCallbacks<ClassT>());
//                    }
//                }
//
//                uint16_t m_Count;
//            };
//
//            template< class ClassT >
//            struct ComponentRegistrar< ClassT, void > : public Reflect::StructureRegistrar< ClassT, void >
//            {
//            public:
//                ComponentRegistrar(const tchar_t* name);
//
//                virtual void Register();
//            };
//            
//#if HELIUM_HEAP
//            HELIUM_ENGINE_API extern Helium::DynamicMemoryHeap g_ComponentAllocator;
//#else
//            static Helium::DefaultAllocator g_ComponentAllocator;
//#endif
//            
//            // The purpose of this adapter is to allow our non-template .cpp code to do the work rather than being
//            // forced to generate many copies of the code we'd want to run
//            struct IComponentContainerAdapter
//            {
//                virtual void Add(Component *_component) = 0;
//                virtual void Reserve(size_t _count) = 0;
//            };
//
//            template <class T>
//            IComponentContainerAdapter *CreateComponentContainerAdapter(T &_container);
//
//            template <class T>
//            IComponentContainerAdapter *CreateComponentContainerAdapter(DynamicArray<T*> &_container);
//
//
//            // Implements find functions
//            HELIUM_ENGINE_API Component* InternalFindOneComponent(ComponentSet &_host, TypeId _type_id, bool _implements);
//            HELIUM_ENGINE_API void InternalFindAllComponents(ComponentSet &_host, TypeId _type_id, bool _implements, IComponentContainerAdapter &_components);
//            
//            // Get ALL components of a type
//            HELIUM_ENGINE_API void InternalGetAllComponents(TypeId _type_id, bool _implements, IComponentContainerAdapter &_components);
//
//            //! A component must be registered. Each component must be registered with reflect. _count indicates max instances of this type.
//
//            /// NOTE: Preferred method of doing this is ComponentType::RegisterComponentType(system, count)
//            ///
//            //HELIUM_ENGINE_API TypeId RegisterType(const Reflect::Class *_class, Private::TypeData &_type_data, Private::TypeData *_base_type_data, uint16_t _count);
//            HELIUM_ENGINE_API TypeId        RegisterType(const Reflect::Structure *_structure, TypeData &_type_data, TypeData *_base_type_data, uint16_t _count, void *_data, IComponentTypeTCallbacks *_callback );
//            HELIUM_ENGINE_API void RegisterComponentPtr(ComponentPtrBase &_ptr_base);
//
//            //! Returns the component to its pool. Old handles to this component will no longer be valid.
//
//            /// Call ReleaseComponent() on the component rather than free directly
//            HELIUM_ENGINE_API void        Free(Component &_component);
//        }
//
//        template <class T>
//        TypeId GetType();
//
//        //! Provides a component to the caller of the given type, attached to the given host. Init data is passed.
//        HELIUM_ENGINE_API Component*  Allocate(IHasComponents &_host, TypeId _type);
//
//        //! Check that _implementor implements _implementee
//        HELIUM_ENGINE_API bool        TypeImplementsType(TypeId _implementor, TypeId _implementee);
//
//        inline Component*  FindFirstComponent(ComponentSet &_set, TypeId _type);
//
//        inline Component*  FindOneComponentThatImplements(ComponentSet &_set, TypeId _type);
//
//        inline void        FindAllComponents(ComponentSet &_set, TypeId _type, DynamicArray<Component *> &_components);
//
//        inline void        FindAllComponentsThatImplement(ComponentSet &_set, TypeId _type, DynamicArray<Component *> &_components);
//        
//        inline void        GetAllComponents(TypeId _type, DynamicArray<Component *> &_components);
//
//        inline void        GetAllComponentsThatImplement(TypeId _type, DynamicArray<Component *> &_components);
//
//        //! Must be called before creating any systemsb
//        HELIUM_ENGINE_API void Initialize();
//
//        //! Call to tear down the component system
//        HELIUM_ENGINE_API void Cleanup();
//
//        HELIUM_ENGINE_API void ProcessPendingDeletes();
//
//        template <class T>        T*  Allocate(IHasComponents &_host);
//
//        HELIUM_ENGINE_API void RemoveAllComponents(ComponentSet &_set);
//
//        template <class T>        T*  FindFirstComponent(ComponentSet &_set);
//
//        template <class T>        T*  FindOneComponentThatImplements(ComponentSet &_set);
//
//        template <class T>        void FindAllComponents(ComponentSet &_set, DynamicArray<T *> &_components);
//
//        template <class T>        void FindAllComponentsThatImplement(ComponentSet &_set, DynamicArray<T *> &_components);
//                
//        template <class T>        void GetAllComponents(DynamicArray<T *> &_components);
//
//        template <class T>        void GetAllComponentsThatImplement(DynamicArray<T *> &_components);
//
//        HELIUM_ENGINE_API size_t CountAllComponents(const TypeId _type_id);
//        HELIUM_ENGINE_API size_t CountAllComponentsThatImplement(const TypeId _type_id);
//
//        template <class T>
//        size_t CountAllComponents()
//        {
//            CountAllComponents(GetType<T>());
//        }
//
//        template <class T>
//        size_t CountAllComponentsThatImplement()
//        {
//            CountAllComponentsThatImplement(GetType<T>());
//        }
//
//        //     template <class T>
//        //     T*  FindComponentsThatImplement(ComponentSet &_set, DynamicArray<Component *> _components)
//        //     {
//        //         return FindFirstComponent(_host, GetType<T>);
//        //     }
//
//                //! All components have some data for bookkeeping
//        class HELIUM_ENGINE_API Component
//        {
//        public:
//            HELIUM_DECLARE_BASE_COMPONENT( Helium::Components::Component )
//            static void PopulateStructure( Reflect::Structure& comp ) { }
//
//            //static void AcceptCompositeVisitor( Reflect::Structure& comp );
//            
//            inline bool operator==( const Component& _rhs ) const { return true; }
//            inline bool operator!=( const Component& _rhs ) const { return true; }
//
//            void MarkForDeletion()
//            {
//                HELIUM_ASSERT(m_OwningSet);
//                Private::RemoveFromChain(this); 
//                Helium::Components::Private::Free(*this);
//            }
//
//            virtual ~Component() { }
//
//			inline Components::ComponentInterface GetComponentInterface() 
//			{ 
//				HELIUM_ASSERT(m_OwningSet); 
//				return Components::ComponentInterface(*m_OwningSet); 
//			}
//
//            // TODO: We could move a lot of this into parallel array with the actual component to get the component sizes far smaller
//            IHasComponents* m_OwningSet;        //< Need pointer back to our owning set in order to detach ourselves from it
//            
//            TypeId          m_TypeId;           //< TypeId.. will eventually be polymorphic pointer to ComponentType<T>. (NOTE: Actualy, TypeID is smaller than a pointer so maybe keep the ID)
//            uint16_t        m_Next;             //< Next component of this same type
//            uint16_t        m_Previous;         //< Previous component of this same type
//            uint16_t        m_RosterIndex;      //< Index/position of roster entry for this component instance
//            GenerationIndex m_Generation;       //< Incremented on every deallocation to tell when a component has been dealloc'ed and realloc'ed
//            //bool            m_PendingDelete;    //< If true, we will deallocate at end of frame
//
//            static Private::ComponentRegistrar<Component, void> s_ComponentRegistrar;
//        };
//        
//        // Code that need not be template aware goes here
//        class HELIUM_ENGINE_API ComponentPtrBase
//        {
//        public:
//            void Check() const
//            {
//               // If no component, we're done
//               if (!m_Component)
//               {
//                   return;
//               }
//
//               // If generation doesn't match
//               if (m_Component->m_Generation != m_Generation)
//               {
//                   // Drop the component
//                   Set(NULL);
//               }
//            }
//
//            bool IsGood() const
//            {
//                Check();
//                return (m_Component != NULL);
//            }
//
//            void Set(Component *_component)
//            {
//				const ComponentPtrBase *ptr = this;
//                ptr->Set(_component);
//            }
//
//            ComponentPtrBase *GetNextComponetPtr() { return m_Next; }
//            friend void Helium::Components::Private::RegisterComponentPtr(ComponentPtrBase &);
//            friend void Helium::Components::ProcessPendingDeletes();
//
//        protected:
//            ComponentPtrBase()
//            : m_Next(0),
//              m_Previous(0),
//              m_Component(0),
//              m_ComponentPtrRegistryHeadIndex(Helium::Invalid<uint16_t>())
//            { }
//
//            ~ComponentPtrBase()
//            {
//                Unlink();
//            }
//
//            void Set(Component *_component) const
//            {
//                if (m_Component == _component)
//                {
//                    // Is also desired case for assigning null to null ptr
//                    return;
//                }
//
//                Unlink();
//                HELIUM_ASSERT(m_ComponentPtrRegistryHeadIndex == Helium::Invalid<uint16_t>());
//                m_Component = _component;
//
//                if (m_Component)
//                {
//                    m_Generation = m_Component->m_Generation;
//                    Helium::Components::Private::RegisterComponentPtr(*const_cast<ComponentPtrBase *>(this));
//                    HELIUM_ASSERT(m_ComponentPtrRegistryHeadIndex != Helium::Invalid<uint16_t>());
//                    HELIUM_ASSERT(!m_Next || m_Next->m_ComponentPtrRegistryHeadIndex == Helium::Invalid<uint16_t>());
//                }
//            }
//
//            void Unlink() const;
//            
//            // Component we point to. NOTE: This will ALWAYS be a type T component because 
//            // this class never sets m_Component to anything but NULL. Our non-base template
//            // class is the only way to construct this class or assign a pointer
//            mutable Component *m_Component; 
//
//        private:
//            // Doubly linked list lets us unsplice ourself easily from list of "active" ComponentPtrs
//            mutable ComponentPtrBase *m_Next;
//            mutable ComponentPtrBase *m_Previous;
//            
//            // We set this generation when a component is assigned
//            mutable GenerationIndex m_Generation;
//
//            // If not assigned to Helium::Invalid<uint16_t>(), this node is head of
//            // linked list and is pointed to by g_ComponentPtrRegistry. Destruction of this
//            // Ptr will automatically fix that reference.
//            mutable uint16_t m_ComponentPtrRegistryHeadIndex;
//        };
//    }
//
//    typedef Components::Component Component;
//    
//    // Code that uses T goes here
//    template <class T>
//    class ComponentPtr : public Helium::Components::ComponentPtrBase
//    {
//    public:
//        ComponentPtr()
//        {
//            Set(0);
//        }
//
//        explicit ComponentPtr(T *_component)
//        {
//            Set(_component);
//        }
//        
//        ComponentPtr( const ComponentPtr& _rhs )
//        {
//            Set(_rhs.m_Component);
//        }
//
//        void operator=(T *_component)
//        {
//            Set(_component);
//        }
//
//        // Only safe to call this if you know the component was not deallocated since Check() was called
//        T *UncheckedGet() const
//        {
//            return static_cast<T*>(m_Component);
//        }
//        
//        T *Get()
//        {
//            Check();
//            return UncheckedGet();
//        }
//        
//        const T *Get() const
//        {
//            Check();
//            return UncheckedGet();
//        }
//
//        T &operator*()
//        {
//            return *Get();
//        }
//
//        T *operator->()
//        {
//            return Get();
//        }
//
//    private:
//    };
//}
