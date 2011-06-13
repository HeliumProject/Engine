#pragma once

#include <vector>

#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Object.h"


//TODO: OnAttach/OnDetach events for components
//TODO: API for systems to iterate over allocated components
//TODO: Smart pointer for handles
//TODO: Review compiled code to make sure that iterators, function objects, etc.
//      are getting inlined properly
//TODO: FreeAll could be more optimized
//TODO: Our instances are new'ed X times and pointers are added to vector. We should
//      probably do a vector of instances instead of vector of pointers to instances
namespace Helium
{

  namespace Components
  {
    /////////////////////////////////////////////////////////////////////////////////
    //! Raw handle to a component.

    /// This handle only is good until the component is destroyed. Later instances of
    /// the the component cannot be resolved using this handle.
    ///
    /// Keep typedefs in sync with ComplexHandle Union in implementation file.. 
    /// data sizes must line up. Embeds type, generation count, and instance id 
    /// all in one (see ComplexHandle)
    typedef uint32_t Handle; 
    const static Handle NULL_HANDLE = 0xFFFFFFFF;

    //! Component type id (not the same as the reflect class id).

    /// Do not use more than 10 bits (1023 max) (see ComplexHandle in implementation). 
    /// In general calling code should avoid using this type
    typedef uint16_t TypeId; 
    const static TypeId NULL_TYPE_ID = 0xFFFF;
    /////////////////////////////////////////////////////////////////////////////////

    class Host;
    class Component;
    class System;

    typedef std::vector<Component *> V_Components;

    //! Encapsulates private data the host should have

    /// Any arbitrary type can host components.. the below Host base class is a
    /// convenience class/example of how to set up a class for hosting components
    struct HostContext
    {
      HostContext() : FirstHandle(NULL_HANDLE) { }
      Handle FirstHandle;

      virtual void OnAttach(Component &_component, void *_init_data) { }
      virtual void OnDetach(Component &_component) { }
    };

    namespace Private
    {
      //! Nodes for linked list of components.

      // Each component makes its own node so we don't thrash
      // memory as we allocate/free components
      struct ComponentInstance
      {
        // TODO: Placing the component as a pointer in an array may introduce an extra
        //       cache miss when resolving handles
        Component *Component;

        Handle PreviousHandle;
        Handle NextHandle;
        uint16_t RosterIndex;

        uint8_t Generation;
      };
      typedef std::vector<ComponentInstance> V_ComponentInstances;

      //! Registering a component type creates one of these
      struct ComponentType
      {
        const Reflect::Class*     m_Class;
        uint16_t                  m_Count;
        uint16_t                  m_FirstUnallocatedIndex;
        std::vector<TypeId>       m_ImplementedTypes;
        std::vector<TypeId>       m_ImplementingTypes;

        // parallel arrays
        std::vector<uint16_t>     m_Roster;
        V_ComponentInstances      m_Instances;
      };
      typedef std::vector<ComponentType> V_ComponentTypes;

      extern V_ComponentTypes g_ComponentTypes;

      // Internal representation of the handle
      union ComplexHandle
      {
        // should be 32 bits in size, because that's the size of handle
        struct
        {
          uint16_t TypeId     : 10;  // 1024 types
          uint16_t Generation : 6;   // use-count wrapping at 64 within the handle
          uint16_t Index;            // max 65535 components per type
        } Part;

        // Handle the outside world gets to see
        Handle Whole;
      };

      //! Data attached to any component class (via the DECLARE_COMPONENT macro)

      /// If one day we ever want to have multiple systems at once, we can turn this
      /// into an array, or let the system's array be sparse
      struct TypeData
      {
        TypeData() : TypeId(NULL_TYPE_ID) { }
        TypeId TypeId;
      };

      // Turns handle into a component chain node. Does not check
      // Generation. Inlined for the benefit of iterators
      Private::ComponentInstance &ResolveComponentInstance(Handle _handle);

      // Inserts component in front the given component in its chain
      void InsertIntoChain(Handle _handle, Handle _insertion_point);

      // Splices component out of the chain it is in
      void RemoveFromChain(Handle _handle);

      // Implements find functions
      Component*          InternalFindOneComponent(HostContext &_host, TypeId _type_id, bool _implements);

      //! Efficiently lets user pass in a vector of a specialized type.

      /// Hate doing this but the only other ways I know to convert vector<A *> to vector<B *> is reinterpret_cast
      /// or copying.
      struct IVectorWrapper
      {
        virtual void PushBack(Component *_component) = 0;
        virtual Component **GetFirst() = 0;
        virtual void Resize(size_t _new_size) = 0;
      };

      template <class T>
      class VectorWrapper : public IVectorWrapper, NonCopyable
      {
      public:
        VectorWrapper(std::vector<T *> &_wrapped)
          : m_Vector(_wrapped)
        {

        }

        virtual void PushBack(Component *_component)
        {
          // We lose this compile time checking but we do this check dynamically within the component system
          m_Vector.push_back(Reflect::AssertCast<T>(_component));
        }

        virtual Component **GetFirst()
        {
          T **pp_c = &m_Vector[0];
          return reinterpret_cast<Component **>(pp_c);
        }

        virtual void Resize(size_t _new_size)
        {
          m_Vector.resize(_new_size);
        }
      private:
        std::vector<T *> &m_Vector;
      };

      //! Find all components of the given type (pass _host for those attached to a particular host)
      void        FindAllComponents(HostContext &_host, TypeId _type, IVectorWrapper &_components);
      void        FindAllComponents(TypeId _type, IVectorWrapper &_components);

      //! Find all components that implements the given type (pass _host for those attached to a particular host)
      void        FindAllComponentsThatImplement(HostContext &_host, TypeId _type, IVectorWrapper &_components);
      void        FindAllComponentsThatImplement(TypeId _type, IVectorWrapper &_components);
    }

    //! A component must be registered. Each component must be registered with reflect. _count indicates max instances of this type.

    /// NOTE: Preferred method of doing this is ComponentType::RegisterComponentType(system, count)
    ///
    LUNAR_ENGINE_API TypeId RegisterType(const Reflect::Class *_class, Private::TypeData &_type_data, Private::TypeData *_base_type_data, uint16_t _count);

    template <class T>
    TypeId GetType()
    {
      Private::TypeData &data = T::GetStaticComponentTypeData();
      HELIUM_ASSERT(data.TypeId != NULL_TYPE_ID);

      return data.TypeId;
    }

    //! Resolves the component by handle. If the handle is stale, returns NULL
    LUNAR_ENGINE_API Component*  ResolveHandle(Handle _handle);

    //! Provides a component to the caller of the given type, attached to the given host. Init data is passed.
    LUNAR_ENGINE_API Component*  Allocate(HostContext &_host, TypeId _type, void *_init_data = NULL);

    //! Returns the component to its pool. Old handles to this component will no longer be valid.
    LUNAR_ENGINE_API void        Free(HostContext &_host, Component &_component);

    //! Frees every component in the given host
    LUNAR_ENGINE_API void        FreeAll(HostContext &_host);

    //! Get first component of the given type
    LUNAR_ENGINE_API Component*  FindOneComponent(HostContext &_host, TypeId _type);

    //! Get first component that implements the given type
    Component*  FindOneComponentThatImplements(HostContext &_host, TypeId _type);

    //! Find all components of the given type (pass _host for those attached to a particular host)

    inline void FindAllComponents(HostContext &_host, TypeId _type, V_Components &_components)
    {
      Private::VectorWrapper<Component> vector_wrapper(_components);
      Private::FindAllComponents(_host, _type, vector_wrapper);
    }

    inline void FindAllComponents(TypeId _type, V_Components &_components)
    {
      Private::VectorWrapper<Component> vector_wrapper(_components);
      Private::FindAllComponents(_type, vector_wrapper);
    }

    //! Find all components that implements the given type (pass _host for those attached to a particular host)
    inline void FindAllComponentsThatImplement(HostContext &_host, TypeId _type, V_Components &_components)
    {
      Private::FindAllComponentsThatImplement(_host, _type, Private::VectorWrapper<Component>(_components));
    }

    inline void FindAllComponentsThatImplement(TypeId _type, V_Components &_components)
    {
      Private::FindAllComponentsThatImplement(_type, Private::VectorWrapper<Component>(_components));
    }

    //! Check that _implementor implements _implementee
    LUNAR_ENGINE_API bool        TypeImplementsType(TypeId _implementor, TypeId _implementee);

    // And some typesafe helpers

    //! See non-template version of allocate
    template <class T>
    T *ResolveHandle(Handle _handle)
    {
      return Reflect::AssertCast<T>(ResolveHandle(_handle));
    }

    template <class T>
    T *Allocate(HostContext &_host, void *_init_data = NULL)
    {
      return Reflect::AssertCast<T>(Allocate(_host, GetType<T>(), _init_data));
    }

    template <class T>
    T *FindOneComponent(HostContext &_host)
    {
      return Reflect::AssertCast<T>(FindOneComponent(_host, GetType<T>()));
    }

    template <class T>
    T *FindOneComponentThatImplements(HostContext &_host)
    {
      return Reflect::AssertCast<T>(FindOneComponentThatImplements(_host, GetType<T>()));
    }

    template <class T>
    void FindAllComponents(HostContext &_host, std::vector<T *> &_components)
    {
      FindAllComponents(_host, GetType<T>(), Private::VectorWrapper<T>(_components));
    }

    template <class T>
    void FindAllComponents(std::vector<T *> &_components)
    {
      FindAllComponents(GetType<T>(), Private::VectorWrapper<T>(_components));
    }

    template <class T>
    void FindAllComponentsThatImplement(HostContext &_host, std::vector<T *> &_components)
    {
      FindAllComponentsThatImplement(_host, GetType<T>(), Private::VectorWrapper<T>(_components));
    }

    template <class T>
    void FindAllComponentsThatImplement(std::vector<T *> &_components)
    {
      FindAllComponentsThatImplement(GetType<T>(), Private::VectorWrapper<T>(_components));
    }

    namespace Private
    {
      // Iterator internals
      struct ComponentTypeDoesNotMatter
      {
        static bool Test(TypeId _tested_type)
        {
          // TODO: Make sure get static is inlining properly
          return true;
        }
      };

      template <class T>
      struct ComponentIsType
      {
        static bool Test(TypeId _tested_type)
        {
          // TODO: Make sure get static is inlining properly
          return T::GetStaticComponentTypeData().TypeId == _tested_type;
        }
      };

      template <class T>
      struct ComponentImplementsType
      {
        static bool Test(TypeId _tested_type)
        {
          return Components::TypeImplementsType(_tested_type, T::GetStaticComponentTypeData().TypeId);
        }
      };

      // Iterator that by default runs through every component in the chain
      template <class T, class TypeTest>
      class ComponentIteratorBase
      {
      public:
        typedef ComponentIteratorBase<T, TypeTest> IterBaseType;

        ComponentIteratorBase(Handle _handle)
        {
          if (_handle == NULL_HANDLE)
          {
            m_Instance = 0;
            return;
          }

          m_Instance = &Components::Private::ResolveComponentInstance(_handle);

          Private::ComplexHandle handle;
          handle.Whole = _handle;

          // no need to loop here because Advance() does that for us
          if (!TypeTest::Test(handle.Part.TypeId))
          {
            Advance();
          }
        }

        // Null iterator
        ComponentIteratorBase()
          : m_Instance(0)
        {

        }

        IterBaseType operator++()
        {
          Advance();
          return *this;
        }

        IterBaseType operator++(int _postfix)
        {
          IterBaseType old = *this;
          Advance();
          return old;
        }

        T &operator*()
        {
          NOC_ASSERT(m_Instance);
          return *Reflect::AssertCast<T>(m_Instance->Component);
        }

        T *operator->()
        {
          NOC_ASSERT(m_Instance);
          return Reflect::AssertCast<T>(m_Instance->Component);
        }

        bool operator==(ComponentIteratorBase &_rhs)
        {
          return (m_Instance == _rhs.m_Instance);
        }

        bool operator!=(ComponentIteratorBase &_rhs)
        {
          return !(*this == _rhs);
        }

      private:
        void Advance()
        {
          if (!m_Instance)
          {
            return;
          }

          Private::ComplexHandle handle;
          handle.Whole = m_Instance->NextHandle;

          while (handle.Whole != NULL_HANDLE)
          {
            m_Instance = &ResolveComponentInstance(handle.Whole);

            if (TypeTest::Test(handle.Part.TypeId))
            {
              return;
            }

            handle.Whole = m_Instance->NextHandle;
          }

          m_Instance = 0;
        }

        Private::ComponentInstance*  m_Instance;
      };
    }

    //! Iterator that runs through all components
    class Iterator : public Private::ComponentIteratorBase<Component, Private::ComponentTypeDoesNotMatter>
    {
    public:
      Iterator(HostContext &_host)
        : IterBaseType(_host.FirstHandle)
      {

      }

      Iterator()
        : IterBaseType(NULL_HANDLE)
      {

      }
    };

    //! Iterator that runs through all components of a certain type
    template <class T>
    class OfTypeIterator : public Private::ComponentIteratorBase<T, Private::ComponentIsType<T> >
    { 
    public:
      OfTypeIterator(HostContext &_host)
        : IterBaseType(_host.FirstHandle)
      {

      }

      OfTypeIterator()
        : IterBaseType(NULL_HANDLE)
      {

      }
    };

    //! Iterator that runs through all components implementing a certain type
    template <class T>
    class ImplementsTypeIterator : public Private::ComponentIteratorBase<T, Private::ComponentImplementsType<T> >
    {
    public:
      ImplementsTypeIterator(HostContext &_host)
        : IterBaseType(_host.FirstHandle)
      {

      }

      ImplementsTypeIterator()
        : IterBaseType(NULL_HANDLE)
      {

      }
    };

    class Host;

    //! Any object that will host a component must extend this. It is designed to be MI-friendly.
    class Host : public HostContext
    {
    public:
      //! Detach every component from object.. implementor of Host may want to do this on destruction
      void DetachAllComponents()
      { Components::FreeAll(GetContext()); }

      //! Provides a component to the caller of the given type, attached to the given host. Init data is passed.
      Component*  AttachComponent(TypeId _type, void *_init_data = NULL)
      { Components::Allocate(GetContext(), _type, _init_data); }

      //! Returns the component to its pool. Old handles to this component will no longer be valid.
      void        DetachComponent(Component &_component)
      { Components::Free(GetContext(), _component); }

      //! Get first component of the given type
      Component*  FindOneComponent(TypeId _type)
      { Components::FindOneComponent(GetContext(), _type); }

      //! Get first component that implements the given type
      Component*  FindOneComponentThatImplements(TypeId _type)
      { Components::FindOneComponentThatImplements(GetContext(), _type); }

      template <class T>
      T *AttachComponent(void *_init_data = NULL)
      {
        return Components::Allocate<T>(GetContext(), _init_data);
      }

      template <class T>
      T *FindOneComponent()
      {
        return Components::FindOneComponent<T>(GetContext());
      }

      template <class T>
      T *FindOneComponentThatImplements()
      {
        return Components::FindOneComponentThatImplements<T>(GetContext());
      }

      Iterator ComponentsBegin()
      {
        return Iterator(GetContext());
      }

      Iterator ComponentsEnd()
      {
        return Iterator();
      }

      template <class T>
      OfTypeIterator<T> ComponentsOfTypeBegin()
      {
        return OfTypeIterator<T>(GetContext());
      }

      template <class T>
      OfTypeIterator<T> ComponentsOfTypeEnd()
      {
        return OfTypeIterator<T>();
      }

      template <class T>
      ImplementsTypeIterator<T> ComponentsImplementingTypeBegin()
      {
        return ImplementsTypeIterator<T>(GetContext());
      }

      template <class T>
      ImplementsTypeIterator<T> ComponentsImplementingTypeEnd()
      {
        return ImplementsTypeIterator<T>();
      }

    private:
      friend System;

      HostContext &GetContext()
      {
        return *this;
      }
    };

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
    return Helium::Components::RegisterType(                                    \
    Helium::Reflect::GetClass<__Type>(),                                  \
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
    return Helium::Components::RegisterType(                                    \
    Helium::Reflect::GetClass<__Type>(),                                  \
    __Type::GetStaticComponentTypeData(),                               \
    &__Base::GetStaticComponentTypeData(),                              \
    _count);                                                      \
    }                                                                 

#define OBJECT_DEFINE_COMPONENT( __Type ) \
  REFLECT_DEFINE_OBJECT( __Type )

#define OBJECT_DEFINE_ABSTRACT_COMPONENT( __Type ) \
  REFLECT_DEFINE_ABSTRACT

    //! Base component class
    class LUNAR_ENGINE_API Component : public Reflect::Object
    {
    public:
      Component() : m_Handle(NULL_HANDLE) { }

      //! Get handle of the component. Using a handle is slower than a pointer, but much safer.
      Handle GetHandle() { return m_Handle; }

      REFLECT_DECLARE_ABSTRACT(Component, Reflect::Object);
      _OBJECT_DECLARE_COMPONENT(Component);

      static void AcceptCompositeVisitor( Reflect::Composite& comp );

      //! Called by component system to update the component's handle

      /// TODO: Make this accessible only to the component system by using friend keyword
      void SetHandle(Handle _handle) { m_Handle = _handle; }

    private:

      // Updated when allocated/freed.
      Handle m_Handle;
    };

    template <class T, class U>
    inline void ForEach(U _function)
    {
      std::vector<T *> components;
      Components::FindAllComponents<T>(components);

      std::for_each(
        components.begin(), 
        components.end(),
        _function);
    }

    //! Must be called before creating any systems
    LUNAR_ENGINE_API void Initialize();

    //! Call to tear down the component system
    LUNAR_ENGINE_API void Cleanup();
  }
}
