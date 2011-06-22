#pragma once

#include <vector>

#include "Engine/Engine.h"

#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Container/Map.h"


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
    //! Component type id (not the same as the reflect class id).
    typedef uint16_t TypeId;
    const static TypeId NULL_TYPE_ID = 0xFFFF;
    /////////////////////////////////////////////////////////////////////////////////

    class Host;
    class BaseComponent;
    class System;

    namespace Private
    {
      //! A static member of every component type (instrumented via the DECLARE_COMPONENT macro)

      /// TODO: Avoid this macro by using reflect a bit more directly like AssetClasses do
      struct TypeData
      {
          TypeData() : TypeId(NULL_TYPE_ID) { }
          TypeId TypeId;
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

    typedef std::vector<BaseComponent *> V_Components;
    typedef Helium::Map<TypeId, BaseComponent *> M_Components;

    //! Any object that can have components attached will own a ComponentSet
    struct ComponentSet
    {
        M_Components m_Components;
    };

    //! All components have some data for bookkeeping
    class BaseComponent : public Reflect::Object
    {
        REFLECT_DECLARE_ABSTRACT(BaseComponent, Reflect::Object);
        _OBJECT_DECLARE_COMPONENT(BaseComponent);

        static void AcceptCompositeVisitor( Reflect::Composite& comp );

        TypeId          m_TypeId;           //< TypeId.. will eventually be polymorphic pointer to ComponentType<T>
        BaseComponent*  m_Next;             //< Next component of this same type
        BaseComponent*  m_Previous;         //< Previous component of this same type
        uint16_t        m_RosterIndex;      //< Index/position of roster entry for this component instance
        uint8_t         m_Generation;       //< Incremented on every deallocation to tell when a component has been dealloc'ed and realloc'ed
        ComponentSet*   m_OwningSet;        //< Need pointer back to our owning set in order to detach ourselves from it
        bool            m_ShouldDeallocate; //< If true, we will deallocate at end of frame
    };

    namespace Private
    {
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
        void *                    m_Pool;                   //< Pointer to the memory block that contains our component instances contiguously
      };
      typedef DynArray<ComponentType> A_ComponentTypes;

      //! List of our type metadata
      extern A_ComponentTypes g_ComponentTypes;
      extern DefaultAllocator g_ComponentAllocator;

      // Inserts component in front the given component in its chain
      void          InsertIntoChain(BaseComponent *_insertee, BaseComponent *_next_component);

      // Splices component out of the chain it is in
      void          RemoveFromChain(BaseComponent *_component);

      // Implements find functions
      BaseComponent*    InternalFindFirstComponent(ComponentSet &_host, TypeId _type_id, bool _implements);

      //! A component must be registered. Each component must be registered with reflect. _count indicates max instances of this type.

      /// NOTE: Preferred method of doing this is ComponentType::RegisterComponentType(system, count)
      ///
      //LUNAR_ENGINE_API TypeId RegisterType(const Reflect::Class *_class, Private::TypeData &_type_data, Private::TypeData *_base_type_data, uint16_t _count);
      TypeId        RegisterType(const Reflect::Class *_class, TypeData &_type_data, TypeData *_base_type_data, uint16_t _count, void *_data );

      template <class T>
      TypeId        RegisterType(Private::TypeData &_type_data, Private::TypeData *_base_type_data, uint16_t _count)
      {
        T *components_block = HELIUM_NEW_A(g_ComponentAllocator, T, _count);
        return RegisterType(T::s_Class, _type_data, _base_type_data, _count, components_block);
      }
    }

    template <class T>
    TypeId GetType()
    {
      Private::TypeData &data = T::GetStaticComponentTypeData();
      HELIUM_ASSERT(data.TypeId != NULL_TYPE_ID);

      return data.TypeId;
    }

    //! Provides a component to the caller of the given type, attached to the given host. Init data is passed.
    LUNAR_ENGINE_API BaseComponent*  Allocate(ComponentSet &_host, TypeId _type, void *_init_data = NULL);

    //! Returns the component to its pool. Old handles to this component will no longer be valid.
    LUNAR_ENGINE_API void        Free(ComponentSet &_host, BaseComponent &_component);

    //! Check that _implementor implements _implementee
    LUNAR_ENGINE_API bool        TypeImplementsType(TypeId _implementor, TypeId _implementee);

    //! Must be called before creating any systems
    LUNAR_ENGINE_API void Initialize();

    //! Call to tear down the component system
    LUNAR_ENGINE_API void Cleanup();
  }
}
