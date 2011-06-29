
#include "EnginePch.h"
#include "Engine/Components.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Components;
using namespace Helium::Components::Private;

REFLECT_DEFINE_ABSTRACT(Component);

void Component::AcceptCompositeVisitor( Reflect::Composite& comp )
{

}

const static TypeId MAX_TYPE_ID = 0xFFFF - 1;

////////////////////////////////////////////////////////////////////////
//       System Implementation
////////////////////////////////////////////////////////////////////////

// TypeId indexes into this
Private::A_ComponentTypes         Private::g_ComponentTypes;
Helium::DynamicMemoryHeap         Private::g_ComponentAllocator;

TypeId Components::Private::RegisterType( const Reflect::Class *_class, TypeData &_type_data, TypeData *_base_type_data, uint16_t _count, void *_data, IComponentTypeTCallbacks *_callbacks )
{
    // Some validation of parameters/state
    HELIUM_ASSERT(_class);
    HELIUM_ASSERT(_count == 0 || _class->m_Creator);
    HELIUM_ASSERT(_count >= 0);
    HELIUM_ASSERT(Reflect::Registry::GetInstance());

    // Component must be registered already
    HELIUM_ASSERT(Component::s_Class);
    HELIUM_ASSERT_MSG(_class->IsType(Component::s_Class), (TXT("Component registered that does not actually extend Component")));

    // Add a bookkeeping struct instance for this type of component
    ComponentType component_type_temp;
    g_ComponentTypes.Add(component_type_temp);

    // Cache a reference to the type
    ComponentType &component_type = g_ComponentTypes.GetLast();

    // Set type id on static member of the class so we can easily get this data later
    TypeId type_id = (uint16_t)g_ComponentTypes.GetSize() - 1;

    // Assert that we havn't already registered this type and then set up the data
    HELIUM_ASSERT(_type_data.m_TypeId == NULL_TYPE_ID);
    _type_data.m_TypeId = type_id;

    // Update bookkeeping fields
    component_type.m_Class = _class;
    component_type.m_FirstUnallocatedIndex = 0;
    component_type.m_InstanceSize = component_type.m_Class->m_Size;
    component_type.m_Pool = _data;
    component_type.m_TCallbacks = _callbacks;

    component_type.m_Roster.Resize(_count);

    // If we have a parent
    if (_base_type_data)
    {
        std::vector<TypeId> base_implemented_types = g_ComponentTypes[_base_type_data->m_TypeId].m_ImplementedTypes;

        // Add base to implemented types, and ourselves to base's implementing types
        component_type.m_ImplementedTypes.push_back(_base_type_data->m_TypeId);
        g_ComponentTypes[_base_type_data->m_TypeId].m_ImplementingTypes.push_back(type_id);

        // For all the base's types, 
        for (std::vector<TypeId>::iterator iter = base_implemented_types.begin();
            iter != base_implemented_types.end(); ++iter)
        {
            component_type.m_ImplementedTypes.push_back(*iter);
            g_ComponentTypes[*iter].m_ImplementingTypes.push_back(type_id);
        }
    }

    // Setup the roster and allocate the components in one pass
    for (uint16_t i = 0; 
        i < component_type.m_Roster.GetSize(); ++i)
    {
        component_type.m_Roster[i] = i;
        Component *component = reinterpret_cast<Component *>(reinterpret_cast<char *>(component_type.m_Pool) + (i * component_type.m_InstanceSize));
        component->m_TypeId = type_id;
        component->m_Previous = 0;
        component->m_Next = 0;
        component->m_RosterIndex = i;
        component->m_Generation = 0;
        component->m_OwningSet = 0;
        component->m_ShouldDeallocate = false;
    }

    // Return the component type's id
    HELIUM_ASSERT(type_id <= MAX_TYPE_ID)
        return type_id;
}

Component* Components::Allocate(ComponentSet &_host, TypeId _type, void *_init_data)
{
    // Make sure type id is good
    HELIUM_ASSERT(_type < g_ComponentTypes.GetSize());
    ComponentType &type = g_ComponentTypes[_type];

    // Do we have a free component to allocate?
    if (type.m_FirstUnallocatedIndex >= type.m_Roster.GetSize())
    {
        // Could not allocate the component because we ran out..
        HELIUM_ASSERT_MSG(false, TXT("Could not allocate component of type %d for host %d"), _type, _host);
        return 0;
    }

    // Find out where the component we should allocate is in the roster
    uint16_t roster_index = type.m_FirstUnallocatedIndex++;
    uint16_t component_index = type.m_Roster[roster_index];

    Component *component = reinterpret_cast<Component *>(reinterpret_cast<char *>(type.m_Pool) + (component_index * type.m_InstanceSize));

    // Insert into chain
    M_Components::Iterator iter = _host.m_Components.Find(_type);
    if (iter != _host.m_Components.End())
    {
        InsertIntoChain(component, iter->Second());
    }
    else
    {
        _host.m_Components.Insert(iter, M_Components::ValueType(_type, component));
    }

    component->m_OwningSet = &_host;

    //_host.OnAttach(*instance.Component, _init_data);
    // Should we call back into component here to let it do stuff?
    // Not going to as we have this callback on the host, and the host
    // will pass itself

    return component;
}

void Components::Free( ComponentSet &_host, Component &_component )
{
    // Component is already freed or component doesn't have a good handle for some reason
    HELIUM_ASSERT(_component.m_OwningSet);

    // Cache to save typing
    ComponentType &component_type = g_ComponentTypes[_component.m_TypeId];

    //_host.OnDetach(_component);

    // Remove from the component chain (and special case to fix the first handle on host if this is first component in chain
    //   if (_host.FirstHandle == handle.Whole)
    //   {
    //     _host.FirstHandle = freed_instance.NextHandle;
    //   }
    //_component.m_OwningSet->m_Components[_component.m_TypeId] = _component.m_Next;
    RemoveFromChain(&_component);

    // Increment generation to invalidate old handles
    ++_component.m_Generation;

    // Get roster indices we will manipulate
    uint16_t used_roster_index = _component.m_RosterIndex;
    uint16_t freed_roster_index = --component_type.m_FirstUnallocatedIndex;

    // Move the roster index that we are freeing to the end, swapping with the end
    // index if necessary
    if (used_roster_index == freed_roster_index)
    {
        // Do nothing because the roster index we are freeing is at the end of the list
    }
    else
    {
        // Swap the roster values
        // - component index: this component's roster index
        // - other component's index: highest in-use component's roster index
        //   - i.e. used_roster_index < freed_roster_index
        int component_index = component_type.m_Roster[used_roster_index];
        int other_component_index = component_type.m_Roster[freed_roster_index];
        component_type.m_Roster[used_roster_index] = other_component_index;
        component_type.m_Roster[freed_roster_index] = component_index;

        // Swap the roster index of the highest in-use component and the recently freed component
        Component *highest_in_use_component = reinterpret_cast<Component *>(reinterpret_cast<char *>(component_type.m_Pool) + (other_component_index * component_type.m_InstanceSize));
        _component.m_RosterIndex = freed_roster_index;
        highest_in_use_component->m_RosterIndex = used_roster_index;
    }
}
// 
// void Components::FreeAll( ComponentSet &_host )
// {
//   Handle handle = _host.FirstHandle;
//   while (handle != NULL_HANDLE)
//   {
//     ComponentInstance &ci = ResolveComponentInstance(handle);
//     handle = ci.NextHandle;
// 
//     Free(_host, *ci.Component);
//   }
// }
// 
// Component* Components::FindOneComponent( ComponentSet &_host, TypeId _type )
// {
//   return InternalFindFirstComponent(_host, _type, false);
// }
// 
// Component* Components::FindOneComponentThatImplements( ComponentSet &_host, TypeId _type )
// {
//   return InternalFindFirstComponent(_host, _type, true);
// }

bool Components::TypeImplementsType( TypeId _implementor, TypeId _implementee )
{
    if (_implementor == _implementee)
    {
        return true;
    }

    // If type B was registered before type A, there is no way B implements A
    if (_implementor < _implementee)
    {
        return false;
    }

    std::vector<TypeId> &implemented_types = g_ComponentTypes[_implementor].m_ImplementedTypes;

    return (std::find(implemented_types.begin(), implemented_types.end(), _implementee) != implemented_types.end());
}

////////////////////////////////////////////////////////////////////////
//       System Utility Methods
////////////////////////////////////////////////////////////////////////

void Components::Private::InsertIntoChain(Component *_insertee, Component *_next_component)
{
    // If we are inserting into a 0-length chain do nothing
    if (_next_component)
    {
        // Fix inserted node's next/previous pointers
        _insertee->m_Next = _next_component;
        _insertee->m_Previous = _next_component->m_Previous;

        // Fix previous component's next pointer
        if (_insertee->m_Previous)
        {
            _insertee->m_Previous->m_Next = _insertee;
        }

        // Fix next component's previous pointer
        _next_component->m_Previous = _insertee;
    }
}

void Components::Private::RemoveFromChain(Component *_component)
{
    // If we have a previous node, repoint its next pointer to our next pointer
    if (_component->m_Previous)
    {
        _component->m_Previous->m_Next = _component->m_Next;
    }
    else if (_component->m_Next)
    {
        _component->m_OwningSet->m_Components[_component->m_TypeId] = _component->m_Next;
    }
    else
    {
        _component->m_OwningSet->m_Components.Remove(_component->m_TypeId);
    }

    // If we have a next node, repoint its previous pointer to our previous pointer
    if (_component->m_Next)
    {
        _component->m_Next->m_Previous = _component->m_Previous;
    }

    // wipe our node
    _component->m_Next = NULL;
    _component->m_Previous = NULL;
}

Component* Components::Private::InternalFindFirstComponent( ComponentSet &_host, TypeId _type_id, bool _implements )
{
    // First search for this type explicitly
    {
        M_Components::Iterator iter = _host.m_Components.Find(_type_id);
        if (iter != _host.m_Components.End())
        {
            return iter->Second();
        }
    }

    if (_implements)
    {
        ComponentType &type = g_ComponentTypes[_type_id];
        for (std::vector<uint16_t>::iterator type_iter = type.m_ImplementingTypes.begin();
            type_iter != type.m_ImplementingTypes.end(); ++type_iter)
        {
            //TODO: Remove this assert once I know it doesn't trip
            HELIUM_ASSERT(*type_iter != _type_id);
            M_Components::Iterator component_iter = _host.m_Components.Find(_type_id);
            if (component_iter != _host.m_Components.End())
            {
                return component_iter->Second();
            }
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////
//       System Functions
////////////////////////////////////////////////////////////////////////

namespace
{
    int32_t m_ComponentsInitCount = 0;
}

void Components::Initialize()
{
    // Register base component with reflect
    if (!m_ComponentsInitCount)
    {
        Reflect::RegisterClassType<Components::Component>(TXT("Component"));
        RegisterType<Component>(Component::GetStaticComponentTypeData(), 0, 0);
    }

    ++m_ComponentsInitCount;
}

void Components::Cleanup()
{
    --m_ComponentsInitCount;

    if (!m_ComponentsInitCount)
    {
        for (TypeId type_id = 0; type_id < g_ComponentTypes.GetSize(); ++type_id)
        {
            // Assert no instances are alive
            HELIUM_ASSERT(g_ComponentTypes[type_id].m_FirstUnallocatedIndex == 0);
            g_ComponentTypes[type_id].m_TCallbacks->DestroyComponents(g_ComponentTypes[type_id]);
            delete g_ComponentTypes[type_id].m_TCallbacks;
            g_ComponentTypes[type_id].m_TCallbacks = 0;
        }

        Reflect::UnregisterClassType<Components::Component>();
        g_ComponentTypes.Clear();
    }
}
// 
// void FindAllComponentsOnHost(ComponentSet &_host, TypeId _type, IVectorWrapper &_components, bool _implements)
// {
//   Private::ComplexHandle handle;
//   handle.Whole = _host.FirstHandle;
// 
//   ComponentInstance *instance = 0;
// 
//   while (handle.Whole != NULL_HANDLE)
//   {
//     instance = &ResolveComponentInstance(handle.Whole);
// 
//     if (_type == handle.Part.TypeId ||
//       _implements && Components::TypeImplementsType(handle.Part.TypeId, _type))
//     {
//       _components.PushBack(instance->Component);
//     }
// 
//     handle.Whole = instance->NextHandle;
//   }
// }
// 
// void Components::Private::FindAllComponents( ComponentSet &_host, TypeId _type, IVectorWrapper &_components )
// {
//   FindAllComponentsOnHost(_host, _type, _components, false);
// }
// 
// void Components::Private::FindAllComponentsThatImplement( ComponentSet &_host, TypeId _type, IVectorWrapper &_components )
// {
//   FindAllComponentsOnHost(_host, _type, _components, true);
// }
// 
// void Components::Private::FindAllComponents( TypeId _type, IVectorWrapper &_components )
// {
//   HELIUM_ASSERT(_type < g_ComponentTypes.size());
//   ComponentType &component_type = g_ComponentTypes[_type];
// 
//   // Resize the vector and add components to it
//   _components.Resize(component_type.m_FirstUnallocatedIndex);
//   if (component_type.m_FirstUnallocatedIndex == 0)
//   {
//     return;
//   }
// 
//   Component **components = _components.GetFirst();
// 
//   for (uint16_t index = 0; index < component_type.m_FirstUnallocatedIndex; ++index)
//   {
//     components[index] = component_type.m_Instances[component_type.m_Roster[index]].Component;
//   }
// }
// 
// void Components::Private::FindAllComponentsThatImplement( TypeId _type, IVectorWrapper &_components )
// {
//   HELIUM_ASSERT(_type < g_ComponentTypes.size());
// 
//   uint32_t component_count = 0;
//   uint32_t current_index = 0;
// 
//   ComponentType &this_component_type = g_ComponentTypes[_type];
// 
//   // Count all the components that implement the given type
//   component_count += this_component_type.m_FirstUnallocatedIndex;
// 
//   for (std::vector<TypeId>::iterator iter = g_ComponentTypes[_type].m_ImplementingTypes.begin();
//     iter != g_ComponentTypes[_type].m_ImplementingTypes.end(); ++iter)
//   {
//     ComponentType &component_type = g_ComponentTypes[*iter];
//     component_count += component_type.m_FirstUnallocatedIndex;
//   }
// 
//   // Resize the vector to contain all components
//   _components.Resize(component_count);
//   if (component_count == 0)
//   {
//     return;
//   }
// 
//   Component **components = _components.GetFirst();
// 
//   for (uint16_t roster_index = 0; roster_index < this_component_type.m_FirstUnallocatedIndex; ++roster_index)
//   {
//     components[current_index] = this_component_type.m_Instances[this_component_type.m_Roster[roster_index]].Component;
//     ++current_index;
//   }
// 
//   // Put components in the vector, loop over each type, then each component of that type
//   for (std::vector<TypeId>::iterator iter = g_ComponentTypes[_type].m_ImplementingTypes.begin();
//     iter != g_ComponentTypes[_type].m_ImplementingTypes.end(); ++iter)
//   {  
//     ComponentType &component_type = g_ComponentTypes[*iter];
//     for (uint16_t roster_index = 0; roster_index < component_type.m_FirstUnallocatedIndex; ++roster_index)
//     {
//       components[current_index] = component_type.m_Instances[component_type.m_Roster[roster_index]].Component;
//       ++current_index;
//     }
//   }
// }
// 
// Private::ComponentInstance & Components::Private::ResolveComponentInstance( Handle _handle )
// {
//   Private::ComplexHandle handle;
//   handle.Whole = _handle;
// 
//   return g_ComponentTypes[handle.Part.TypeId].m_Instances[handle.Part.Index];
// }
