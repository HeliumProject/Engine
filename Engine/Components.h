#pragma once

#include <vector>

#include "Engine/Engine.h"

#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Container/Map.h"
#include "Foundation/Memory/AutoPtr.h"

//TODO: OnAttach/OnDetach events for components?
//TODO: API for systems to iterate over allocated components
//TODO: Review compiled code to make sure that iterators, function objects, etc.
//      are getting inlined properly
//TODO: Smart pointer
namespace Helium
{
    namespace Components
    {
        //! Component type id (not the same as the reflect class id).
        typedef uint16_t TypeId;
        const static TypeId NULL_TYPE_ID = 0xFFFF;

        typedef uint8_t GenerationIndex;
        const static uint32_t COMPONENT_PTR_CHECK_FREQUENCY = 256;

        class Host;
        class Component;
        class System;
        class ComponentPtrBase;

        namespace Private
        {
            //! A static member of every component type (instrumented via the DECLARE_COMPONENT macro)

            /// TODO: Avoid this macro by using reflect a bit more directly like AssetClasses do
            struct TypeData
            {
                TypeData() : m_TypeId(NULL_TYPE_ID) { }
                TypeId m_TypeId;
            };
        }

        //Internal use only
#define _OBJECT_DECLARE_COMPONENT( __Type )                         \
    public:                                                             \
    friend Helium::Components::System;                                        \
    /* Every class implementing Component must have this function */  \
    static Helium::Components::Private::TypeData &GetStaticComponentTypeData()      \
        {                                                                 \
        static Helium::Components::Private::TypeData data;                      \
        return data;                                                    \
        }                                                                 \
        \
    public:

        //! Add to any component that will not be instantiated
#define OBJECT_DECLARE_ABSTRACT_COMPONENT( __Type, __Base )         \
    REFLECT_DECLARE_ABSTRACT(__Type, __Base)                          \
    _OBJECT_DECLARE_COMPONENT(__Type)                                 \
    public:                                                             \
    /* Convenience function for registering components. It is   */    \
    /* typesafe to register using this function, so this is the */  \
    /* preferred method */                                          \
    static Helium::Components::TypeId RegisterComponentType()                  \
        {                                                                 \
        return Helium::Components::Private::RegisterType<__Type>(                                    \
        __Type::GetStaticComponentTypeData(),                               \
        &__Base::GetStaticComponentTypeData(),                              \
        0);                                                           \
        }                                                                 

        //! Add to any component that will be instantiated
#define OBJECT_DECLARE_COMPONENT( __Type, __Base )                  \
    REFLECT_DECLARE_OBJECT(__Type, __Base)                             \
    _OBJECT_DECLARE_COMPONENT(__Type)                                 \
    public:                                                             \
    /* Convenience function for registering components. It is   */    \
    /* typesafe to register using this function, so this is the */    \
    /* preferred method */                                            \
    static Helium::Components::TypeId RegisterComponentType(                  \
    uint16_t _count)                                                     \
        {                                                                 \
        return Helium::Components::Private::RegisterType<__Type>(                                    \
        __Type::GetStaticComponentTypeData(),                               \
        &__Base::GetStaticComponentTypeData(),                              \
        _count);                                                      \
        }                                                                 

#define OBJECT_DEFINE_COMPONENT( __Type ) \
    REFLECT_DEFINE_OBJECT( __Type )

#define OBJECT_DEFINE_ABSTRACT_COMPONENT( __Type ) \
    REFLECT_DEFINE_ABSTRACT

        typedef std::vector<Component *> V_Components;
        typedef Helium::Map<TypeId, Component *> M_Components;

        //! Any object that can have components attached will own a ComponentSet
        struct ComponentSet
        {
            M_Components m_Components;
        };

        //! All components have some data for bookkeeping
        class HELIUM_ENGINE_API Component : public Reflect::Object
        {
            REFLECT_DECLARE_ABSTRACT(Component, Reflect::Object);
            _OBJECT_DECLARE_COMPONENT(Component);

            static void AcceptCompositeVisitor( Reflect::Composite& comp );

            void MarkForDeletion()
            {
                HELIUM_ASSERT(m_OwningSet);
                m_PendingDelete = true;
            }

            TypeId          m_TypeId;           //< TypeId.. will eventually be polymorphic pointer to ComponentType<T>
            Component*  m_Next;             //< Next component of this same type
            Component*  m_Previous;         //< Previous component of this same type
            uint16_t        m_RosterIndex;      //< Index/position of roster entry for this component instance
            GenerationIndex m_Generation;       //< Incremented on every deallocation to tell when a component has been dealloc'ed and realloc'ed
            ComponentSet*   m_OwningSet;        //< Need pointer back to our owning set in order to detach ourselves from it
            bool            m_PendingDelete; //< If true, we will deallocate at end of frame
        };

        namespace Private
        {
            //! I want to generate a couple functions that require the T of the components the type contains.
            struct IComponentTypeTCallbacks
            {
                virtual void DestroyComponents(struct ComponentType &_type_info) = 0;
            };

            //! Registering a component type creates one of these
            struct ComponentType
            {
                const Reflect::Class*     m_Class;                  //< Class of the component's type
                size_t                    m_InstanceSize;           //< Cache sizeof(Component Type).. to save frequent pointer resolve of m_Class
                uint16_t                  m_FirstUnallocatedIndex;  //< Pointer to "free" pointer of roster
                std::vector<TypeId>       m_ImplementedTypes;       //< Parent type IDs of this type
                std::vector<TypeId>       m_ImplementingTypes;      //< Child types IDs of this type

                // parallel arrays
                DynArray<uint16_t>        m_Roster;                 //< List of component indeces.. first all allocated then all deallocated components
                void *                    m_Pool;               //< Pointer to the memory block that contains our component instances contiguously
                IComponentTypeTCallbacks *m_TCallbacks;          //< Pointer to template object that implements a couple useful functions.. it's a raw
                //  pointer because auto ptrs can't be copied and this struct goes in a DynArray

            };
            typedef DynArray<ComponentType> A_ComponentTypes;

            template <class T>
            struct ComponentTypeTCallbacks : public IComponentTypeTCallbacks
            {
                virtual void DestroyComponents(ComponentType &_type_info)
                {
                    HELIUM_DELETE_A(g_ComponentAllocator, static_cast<T *>(_type_info.m_Pool));
                    Private::TypeData &data = T::GetStaticComponentTypeData();
                    data.m_TypeId = NULL_TYPE_ID;
                }
            };

            HELIUM_ENGINE_API extern Helium::DynamicMemoryHeap g_ComponentAllocator;

            // Inserts component in front the given component in its chain
            void          InsertIntoChain(Component *_insertee, Component *_next_component);

            // Splices component out of the chain it is in
            void          RemoveFromChain(Component *_component);

            struct IComponentContainerAdapter
            {
                virtual void Add(Component *_component) = 0;
            };

            template <class T>
            IComponentContainerAdapter *CreateComponentContainerAdapter(T &_container);

            template <class T>
            IComponentContainerAdapter *CreateComponentContainerAdapter(DynArray<T*> &_container)
            {
                class DynArrayAdapter : public IComponentContainerAdapter
                {
                public:
                    virtual ~DynArrayAdapter() { }
                    DynArrayAdapter(DynArray<T*> &_array)
                        : m_Array(_array)
                    {

                    }

                    virtual void Add(Component *_component)
                    {
                        T *typed_component = Reflect::AssertCast<T>(_component);
                        m_Array.Add(typed_component);
                    }

                private:
                    DynArray<T*> &m_Array;
                };

                return new DynArrayAdapter(_container);
            }


            // Implements find functions
            HELIUM_ENGINE_API Component*    InternalFindOneComponent(ComponentSet &_host, TypeId _type_id, bool _implements);

            HELIUM_ENGINE_API Component*    InternalFindAllComponents(ComponentSet &_host, TypeId _type_id, bool _implements, IComponentContainerAdapter &_components);



            //! A component must be registered. Each component must be registered with reflect. _count indicates max instances of this type.

            /// NOTE: Preferred method of doing this is ComponentType::RegisterComponentType(system, count)
            ///
            //HELIUM_ENGINE_API TypeId RegisterType(const Reflect::Class *_class, Private::TypeData &_type_data, Private::TypeData *_base_type_data, uint16_t _count);
            HELIUM_ENGINE_API TypeId        RegisterType(const Reflect::Class *_class, TypeData &_type_data, TypeData *_base_type_data, uint16_t _count, void *_data, IComponentTypeTCallbacks *_callback );

            template <class T>
            TypeId        RegisterType(Private::TypeData &_type_data, Private::TypeData *_base_type_data, uint16_t _count)
            {
                T *components_block = HELIUM_NEW_A(g_ComponentAllocator, T, _count);
                TypeId type_id = RegisterType(T::s_Class, _type_data, _base_type_data, _count, components_block, new ComponentTypeTCallbacks<T>());
                return type_id;
            }

            HELIUM_ENGINE_API void RegisterComponentPtr(ComponentPtrBase &_ptr_base);

            //! Returns the component to its pool. Old handles to this component will no longer be valid.

            /// This is now private because component has MarkForDeletion(), as we now will be doing a sweep through components
            /// to free them once per frame
            HELIUM_ENGINE_API void        Free(Component &_component);
        }

        template <class T>
        TypeId GetType()
        {
            Private::TypeData &data = T::GetStaticComponentTypeData();
            HELIUM_ASSERT(data.m_TypeId != NULL_TYPE_ID);

            return data.m_TypeId;
        }

        //! Provides a component to the caller of the given type, attached to the given host. Init data is passed.
        HELIUM_ENGINE_API Component*  Allocate(ComponentSet &_host, TypeId _type, void *_init_data = NULL);

        //! Check that _implementor implements _implementee
        HELIUM_ENGINE_API bool        TypeImplementsType(TypeId _implementor, TypeId _implementee);

        inline Component*  FindOneComponent(ComponentSet &_set, TypeId _type)
        {
            return Private::InternalFindOneComponent(_set, _type, false);
        }

        inline Component*  FindOneComponentThatImplements(ComponentSet &_set, TypeId _type)
        {
            return Private::InternalFindOneComponent(_set, _type, true);
        }

        inline void        FindAllComponents(ComponentSet &_set, TypeId _type, DynArray<Component *> &_components)
        {
            //AutoPtr<Private::IComponentContainerAdapter> container;
            Private::IComponentContainerAdapter *container;
            container = Private::CreateComponentContainerAdapter(_components);
            Private::InternalFindAllComponents(_set, _type, false, *container);
            delete container;
        }

        inline void        FindAllComponentsThatImplement(ComponentSet &_set, TypeId _type, DynArray<Component *> &_components)
        {
            //AutoPtr<Private::IComponentContainerAdapter> container;
            Private::IComponentContainerAdapter *container;
            container = Private::CreateComponentContainerAdapter(_components);
            Private::InternalFindAllComponents(_set, _type, true, *container);
            delete container;
        }

        //HELIUM_ENGINE_API void        FindComponentsThatImplement(ComponentSet &_set, TypeId _type, DynArray<Component *> _components);

        //! Must be called before creating any systemsb
        HELIUM_ENGINE_API void Initialize();

        //! Call to tear down the component system
        HELIUM_ENGINE_API void Cleanup();

        HELIUM_ENGINE_API void ProcessPendingDeletes();

        template <class T>
        T*  Allocate(ComponentSet &_host, void *_init_data = NULL)
        {
            return static_cast<T *>(Allocate(_host, GetType<T>(), _init_data));
        }

        template <class T>
        T*  FindOneComponent(ComponentSet &_set)
        {
            return static_cast<T *>(FindOneComponent(_set, GetType<T>()));
        }

        template <class T>
        T*  FindOneComponentThatImplements(ComponentSet &_set)
        {
            return static_cast<T *>(FindOneComponentThatImplements(_set, GetType<T>()));
        }

        template <class T>
        void FindAllComponents(ComponentSet &_set, DynArray<T *> &_components)
        {
            //AutoPtr<Private::IComponentContainerAdapter> container;
            Private::IComponentContainerAdapter *container;
            container = Private::CreateComponentContainerAdapter<T>(_components);
            Private::InternalFindAllComponents(_set, GetType<T>(), false, *container);
            delete container;
        }

        template <class T>
        void FindAllComponentsThatImplement(ComponentSet &_set, DynArray<T *> &_components)
        {
            //AutoPtr<Private::IComponentContainerAdapter> container;
            Private::IComponentContainerAdapter *container;
            container = Private::CreateComponentContainerAdapter<T>(_components);
            Private::InternalFindAllComponents(_set, GetType<T>(), true, *container);
            delete container;
        }

        //     template <class T>
        //     T*  FindComponentsThatImplement(ComponentSet &_set, DynArray<Component *> _components)
        //     {
        //         return FindOneComponent(_host, GetType<T>);
        //     }

        // Code that need not be template aware goes here
        class HELIUM_ENGINE_API ComponentPtrBase
        {
        public:
            void Check()
            {
               // If no component, we're done
               if (!m_Component)
               {
                   return;
               }

               // If generation doesn't match
               if (m_Component->m_Generation != m_Generation)
               {
                   // Drop the component
                   AssignComponent(NULL);
               }
            }

            bool IsGood()
            {
                Check();
                return (m_Component != NULL);
            }

            void Unlink();

            void AssignComponent(Component *_component)
            {
                if (m_Component == _component)
                {
                    // Is also desired case for assigning null to null ptr
                    return;
                }

                Unlink();
                m_Component = _component;

                if (m_Component)
                {
                    m_Generation = m_Component->m_Generation;
                    Helium::Components::Private::RegisterComponentPtr(*this);
                }
            }

            ComponentPtrBase *GetNextComponetPtr() { return m_Next; }
            friend void Helium::Components::Private::RegisterComponentPtr(ComponentPtrBase &);
            friend void Helium::Components::ProcessPendingDeletes();

        protected:
            ComponentPtrBase()
            : m_Next(0),
              m_Previous(0),
              m_Component(0),
              m_ComponentPtrRegistryHeadIndex(Helium::Invalid<uint32_t>())
            { }            
            
            // Component we point to. NOTE: This will ALWAYS be a type T component because 
            // this class never sets m_Component to anything but NULL. Our non-base template
            // class is the only way to construct this class or assign a pointer
            Component *m_Component; 

        private:
            // Doubly linked list lets us unsplice ourself easily from list of "active" ComponentPtrs
            ComponentPtrBase *m_Next;
            ComponentPtrBase *m_Previous;
            
            // We set this generation when a component is assigned
            GenerationIndex m_Generation;

            // If not assigned to Helium::Invalid<uint32_t>(), this node is head of
            // linked list and is pointed to by g_ComponentPtrRegistry. Destruction of this
            // Ptr will automatically fix that reference.
            uint32_t m_ComponentPtrRegistryHeadIndex;
        };

        // Code that uses T goes here
        template <class T>
        class ComponentPtr : public ComponentPtrBase
        {
        public:
            ComponentPtr()
            {
                AssignComponent(0);
            }

            ComponentPtr(T *_component)
            {
                AssignComponent(_component);
            }

            T &operator*()
            {
                Check();
                return *static_cast<T*>(m_Component);
            }

            T *operator->()
            {
                Check();
                return static_cast<T*>(m_Component);
            }


        private:
        };

        //class ComponentPtrTracker
        //{
        //public:
        //    void CheckPointersValid(u8 _frame_count)
        //    {
        //        uint32_t list_index = _frame_count % COMPONENT_PTR_CHECK_FREQUENCY;
        //        
        //        ComponentPtr *component_ptr = m_Ptr[list_index];
        //        while (component_ptr)
        //        {
        //            component_ptr->Check();
        //        }
        //    }

        //private:
        //    ComponentPtrBase *m_Ptr[COMPONENT_PTR_CHECK_FREQUENCY];
        //};

        class HasComponents
        {
        public:

            template <class T>
            T*  Allocate(void *_init_data = NULL)
            {
                return Helium::Components::Allocate<T>(m_Components, _init_data);
            }

            template <class T>
            T*  FindOneComponent()
            {
                return Helium::Components::FindOneComponent<T>(m_Components);
            }

            template <class T>
            T*  FindOneComponentThatImplements()
            {
                return Helium::Components::FindOneComponentThatImplements<T>(m_Components);
            }

            template <class T>
            void FindAllComponents(DynArray<T *> &_components)
            {
                Helium::Components::FindAllComponents<T>(m_Components, _components);
            }

            template <class T>
            void FindAllComponentsThatImplement(DynArray<T *> &_components)
            {
                Helium::Components::FindAllComponentsThatImplement<T>(m_Components, _components);
            }

            //     template <class T>
            //     T*  FindComponentsThatImplement(ComponentSet &_set, DynArray<Component *> _components)
            //     {
            //         return FindComponentsThatImplement(_host, GetType<T>);
            //     }

        protected:
            ComponentSet m_Components;
        };
    }
}
