
#include "EnginePch.h"
#include "Engine/Components.h"

#include "Reflect/Data/DataDeduction.h"
#include "Engine/AssetType.h"

REFLECT_DEFINE_BASE_STRUCTURE(Helium::Components::Component);

using namespace Helium;
using namespace Helium::Components;
using namespace Helium::Components::Private;

inline Helium::Components::Component *GetComponentFromIndex(ComponentType &_type, uint32_t _index)
{
    return reinterpret_cast<Helium::Components::Component *>(reinterpret_cast<char *>(_type.m_Pool) + (_index * _type.m_InstanceSize));
}

const static TypeId MAX_TYPE_ID = 0xFFFF - 1;

////////////////////////////////////////////////////////////////////////
//       System Implementation
////////////////////////////////////////////////////////////////////////

Helium::DynamicMemoryHeap         Private::g_ComponentAllocator;

// TypeId indexes into this
namespace
{
    int32_t g_ComponentsInitCount = 0;
    Private::A_ComponentTypes         g_ComponentTypes;
    ComponentPtrBase*                 g_ComponentPtrRegistry[COMPONENT_PTR_CHECK_FREQUENCY];
    uint32_t                          g_ComponentProcessPendingDeletesCallCount = 0;
}

inline uint16_t ConvertPtrToIndex(Helium::Component *_ptr)
{
    HELIUM_ASSERT(_ptr);

    size_t value = (reinterpret_cast<size_t>(_ptr) - reinterpret_cast<size_t>(g_ComponentTypes[_ptr->m_TypeId].m_Pool)) 
            / g_ComponentTypes[_ptr->m_TypeId].m_InstanceSize;

    HELIUM_ASSERT(value < Invalid<uint16_t>());
    return static_cast<uint16_t>(value);
}

inline Helium::Component *ConvertIndexToPtr(uint16_t _index, TypeId _type)
{
    HELIUM_ASSERT(_index != Invalid<uint16_t>());
    HELIUM_ASSERT(_type < g_ComponentTypes.GetSize());
    return reinterpret_cast<Helium::Component *>(reinterpret_cast<size_t>(g_ComponentTypes[_type].m_Pool) + g_ComponentTypes[_type].m_InstanceSize * _index);
}

TypeId Components::Private::RegisterType( const Reflect::Structure *_structure, TypeData &_type_data, TypeData *_base_type_data, uint16_t _count, void *_data, IComponentTypeTCallbacks *_callbacks )
{
    // Some validation of parameters/state
    HELIUM_ASSERT(_structure);
    //HELIUM_ASSERT(_count == 0 || _structure->m_Creator);
    HELIUM_ASSERT(_count >= 0);
    HELIUM_ASSERT(Reflect::Registry::GetInstance());
    HELIUM_ASSERT(!_base_type_data || _base_type_data->m_TypeId != NULL_TYPE_ID);
    
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
    component_type.m_Structure = _structure;
    component_type.m_FirstUnallocatedIndex = 0;
    component_type.m_InstanceSize = component_type.m_Structure->m_Size;
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
        Component *component = GetComponentFromIndex(component_type, i);
        component->m_TypeId = type_id;
        component->m_Previous = Invalid<uint16_t>();
        component->m_Next = Invalid<uint16_t>();
        component->m_RosterIndex = i;
        component->m_Generation = 0;
        component->m_OwningSet = 0;
        //component->m_PendingDelete = false;
    }

    // Return the component type's id
    HELIUM_ASSERT(type_id <= MAX_TYPE_ID)
        return type_id;
}

Helium::Components::Component* Components::Allocate(Components::IHasComponents &_host, TypeId _type)
{
    ComponentSet &component_set = _host.GetComponentSet();

    // Make sure type id is good
    HELIUM_ASSERT(_type < g_ComponentTypes.GetSize());
    ComponentType &type = g_ComponentTypes[_type];

    // Do we have a free component to allocate?
    if (type.m_FirstUnallocatedIndex >= type.m_Roster.GetSize())
    {
        // Could not allocate the component because we ran out..
        HELIUM_ASSERT_MSG(type.m_FirstUnallocatedIndex < type.m_Roster.GetSize(), TXT("Could not allocate component of type %d for host %d"), _type, _host);
        return 0;
    }

    // Find out where the component we should allocate is in the roster
    uint16_t roster_index = type.m_FirstUnallocatedIndex++;
    uint16_t component_index = type.m_Roster[roster_index];

    Component *component = GetComponentFromIndex(type, component_index);

    // Insert into chain
    M_Components::Iterator iter = component_set.m_Components.Find(_type);
    if (iter != component_set.m_Components.End())
    {
        InsertIntoChain(component, iter->Second());
        iter->Second() = component;
    }
    else
    {
        component_set.m_Components.Insert(iter, M_Components::ValueType(_type, component));
    }

    component->m_OwningSet = &_host;

    //HELIUM_ASSERT(!component->m_PendingDelete);

    type.m_TCallbacks->CallConstructor(component);

    return component;
}

void Components::Private::Free( Component &_component )
{
    _component.~Component();

    // Component is already freed or component doesn't have a good handle for some reason
    HELIUM_ASSERT(_component.m_OwningSet);

    // Cache to save typing
    ComponentType &component_type = g_ComponentTypes[_component.m_TypeId];
    
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
        uint16_t component_index = component_type.m_Roster[used_roster_index];
        uint16_t other_component_index = component_type.m_Roster[freed_roster_index];
        component_type.m_Roster[used_roster_index] = other_component_index;
        component_type.m_Roster[freed_roster_index] = component_index;

        // Swap the roster index of the highest in-use component and the recently freed component
        Component *highest_in_use_component = GetComponentFromIndex(component_type, other_component_index);
        _component.m_RosterIndex = freed_roster_index;
        highest_in_use_component->m_RosterIndex = used_roster_index;
    }
}

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
        _insertee->m_Next = ConvertPtrToIndex(_next_component);
        _insertee->m_Previous = _next_component->m_Previous;

        // Fix previous component's next pointer
        if (_insertee->m_Previous != Invalid<uint16_t>())
        {
            ConvertIndexToPtr(_insertee->m_Previous, _insertee->m_TypeId)->m_Next = ConvertPtrToIndex(_insertee);
        }

        // Fix next component's previous pointer
        _next_component->m_Previous = ConvertPtrToIndex(_insertee);
    }
}

void Components::Private::RemoveFromChain(Component *_component)
{
    // Components were already unlinked. Calls to Mark
    if (!_component->m_OwningSet)
    {
        return;
    }

    // If we have a previous node, repoint its next pointer to our next pointer
    if (_component->m_Previous != Invalid<uint16_t>())
    {
        ConvertIndexToPtr(_component->m_Previous, _component->m_TypeId)->m_Next = _component->m_Next;
    }
    else if (_component->m_Next != Invalid<uint16_t>())
    {
        _component->m_OwningSet->GetComponentSet().m_Components[_component->m_TypeId] = ConvertIndexToPtr(_component->m_Next, _component->m_TypeId);
    }
    else
    {
        _component->m_OwningSet->GetComponentSet().m_Components.Remove(_component->m_TypeId);
    }

    // If we have a next node, repoint its previous pointer to our previous pointer
    if (_component->m_Next != Invalid<uint16_t>())
    {
        ConvertIndexToPtr(_component->m_Next, _component->m_TypeId)->m_Previous = _component->m_Previous;
    }

    // wipe our node
    _component->m_Next = Invalid<uint16_t>();
    _component->m_Previous = Invalid<uint16_t>();
}

Helium::Components::Component* Components::Private::InternalFindOneComponent( ComponentSet &_host, TypeId _type_id, bool _implements )
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
            Component *c = InternalFindOneComponent(_host, *type_iter, false);
            if (c)
            {
                return c;
            }
        }
    }

    return 0;
}

void Components::Private::InternalFindAllComponents( ComponentSet &_host, TypeId _type_id, bool _implements, IComponentContainerAdapter &_components )
{
    // First search for this type explicitly
    {
        M_Components::Iterator iter = _host.m_Components.Find(_type_id);
        if (iter != _host.m_Components.End())
        {
            Component *c = iter->Second();
            while (c)
            {
                _components.Add(c);
                c = (c->m_Next != Invalid<uint16_t>()) ? ConvertIndexToPtr(c->m_Next, c->m_TypeId) : NULL;
            }
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
            InternalFindAllComponents(_host, *type_iter, false, _components);
        }
    }
}

void Helium::Components::Private::InternalGetAllComponents( TypeId _type_id, bool _implements, IComponentContainerAdapter &_components )
{
    HELIUM_ASSERT(g_ComponentTypes.GetSize() > _type_id);

    size_t count = g_ComponentTypes[_type_id].m_FirstUnallocatedIndex;
    if (_implements)
    {
        ComponentType &type = g_ComponentTypes[_type_id];
        for (std::vector<uint16_t>::iterator type_iter = type.m_ImplementingTypes.begin();
            type_iter != type.m_ImplementingTypes.end(); ++type_iter)
        {
            count += type.m_FirstUnallocatedIndex;
        }
    }

    _components.Reserve(count);

    {
        ComponentType &type = g_ComponentTypes[_type_id];
        for (uint16_t i = 0; i < type.m_FirstUnallocatedIndex; ++i)
        {
            _components.Add(GetComponentFromIndex(type, type.m_Roster[i]));
        }
    }

    if (_implements)
    {
        ComponentType &type = g_ComponentTypes[_type_id];
        for (std::vector<uint16_t>::iterator type_iter = type.m_ImplementingTypes.begin();
            type_iter != type.m_ImplementingTypes.end(); ++type_iter)
        {
            InternalGetAllComponents(*type_iter, false, _components);
        }
    }
}

////////////////////////////////////////////////////////////////////////
//       System Functions
////////////////////////////////////////////////////////////////////////


void Components::Initialize()
{
    // Register base component with reflect
    if (!g_ComponentsInitCount)
    {
        RegisterType<Component>(Component::GetStaticComponentTypeData(), 0, 0);
    }

    ++g_ComponentsInitCount;
}

void Components::Cleanup()
{
    --g_ComponentsInitCount;

    if (!g_ComponentsInitCount)
    {
        ProcessPendingDeletes();
        for (TypeId type_id = 0; type_id < g_ComponentTypes.GetSize(); ++type_id)
        {
            // Assert no instances are alive
            HELIUM_ASSERT(g_ComponentTypes[type_id].m_FirstUnallocatedIndex == 0);
            g_ComponentTypes[type_id].m_TCallbacks->DestroyComponents(g_ComponentTypes[type_id]);
            delete g_ComponentTypes[type_id].m_TCallbacks;
            g_ComponentTypes[type_id].m_TCallbacks = 0;
        }

        g_ComponentTypes.Clear();
    }
}

HELIUM_ENGINE_API void Helium::Components::ProcessPendingDeletes()
{
    ++g_ComponentProcessPendingDeletesCallCount;

    // Delete all components that have m_PendingDelete flag set to true
    //for (DynamicArray<ComponentType>::Iterator iter = g_ComponentTypes.Begin();
    //    iter != g_ComponentTypes.End(); ++iter)
    //{
    //    // Algorithm could be "skip around, looking at allocated components" or "look at all components in sequence
    //    // in memory. Going for naive first approach because it's less fancy and more direct. If we usually
    //    // have close to 100% component utilization the other way might be better (it's cache friendly)
    //    for (int i = iter->m_FirstUnallocatedIndex - 1; i >= 0; --i)
    //    {
    //        Component *c = GetComponentFromIndex(*iter, iter->m_Roster[i]);
    //        if (c->m_PendingDelete)
    //        {
    //            // NOTE: component knows about its owning set so we are removing it from that set by calling Free
    //            Helium::Components::Private::Free(*c);
    //            c->m_PendingDelete = false;
    //        }
    //    }
    //}

    // Look at our registry of component ptrs, we may need to force some of them to invalidate (a ptr must be checked
    // at least once every 256 frames in case the generation counter overlaps)
    uint32_t registry_index = g_ComponentProcessPendingDeletesCallCount % COMPONENT_PTR_CHECK_FREQUENCY;
    ComponentPtrBase *component_ptr = g_ComponentPtrRegistry[registry_index];

    // Double-check that the component head index is set
    HELIUM_ASSERT(!component_ptr || component_ptr->m_ComponentPtrRegistryHeadIndex == registry_index);

    while (component_ptr)
    {
        // Don't do the check until we get our next pointer, as a failed check will splice out the ptr from our doubly
        // linked list.
        ComponentPtrBase *component_ptr_to_check = component_ptr;
        component_ptr = component_ptr->GetNextComponetPtr();
        component_ptr_to_check->Check();
    }
    
    // Even after we evict components, we still need to verify that we either have no component or the head component
    // has a properly set index
    HELIUM_ASSERT(
        !g_ComponentPtrRegistry[registry_index] || 
        g_ComponentPtrRegistry[registry_index]->m_ComponentPtrRegistryHeadIndex == registry_index);
}

void Helium::Components::RemoveAllComponents(ComponentSet &_set)
{
    while (_set.m_Components.GetSize() > 0)
    {
        // Get the first entry because unlinking all components destroys that map entry
        Component *c = _set.m_Components.Begin()->Second();
        while (c)
        {
            Component *next = ConvertIndexToPtr(c->m_Next, c->m_TypeId);
            c->MarkForDeletion();
            c = next;
        }
    }
}

HELIUM_ENGINE_API size_t Helium::Components::CountAllComponents( const TypeId _type_id )
{
    HELIUM_ASSERT(_type_id < g_ComponentTypes.GetSize());
    return g_ComponentTypes[_type_id].m_FirstUnallocatedIndex;
}

HELIUM_ENGINE_API size_t Helium::Components::CountAllComponentsThatImplement( const TypeId _type_id )
{
    size_t count = CountAllComponents(_type_id);

    std::vector<TypeId> &implementing_types = g_ComponentTypes[_type_id].m_ImplementingTypes;
    for (std::vector<TypeId>::iterator iter = implementing_types.begin(); 
        iter != implementing_types.end(); ++iter)
    {
        count += CountAllComponents(*iter);
    }

    return count;
}

void Helium::Components::Private::RegisterComponentPtr( ComponentPtrBase &_ptr_base )
{
    uint32_t registry_index = g_ComponentProcessPendingDeletesCallCount % COMPONENT_PTR_CHECK_FREQUENCY;
    _ptr_base.m_Next = g_ComponentPtrRegistry[registry_index];
    
    if (_ptr_base.m_Next)
    {
        // Make the current head component's previous pointer point to the new component, and clear the head component index
        HELIUM_ASSERT(_ptr_base.m_Next->m_ComponentPtrRegistryHeadIndex == registry_index);
        _ptr_base.m_Next->m_Previous = &_ptr_base;
        _ptr_base.m_Next->m_ComponentPtrRegistryHeadIndex = Helium::Invalid<uint32_t>();
    }

    _ptr_base.m_ComponentPtrRegistryHeadIndex = registry_index;
    g_ComponentPtrRegistry[registry_index] = &_ptr_base;
}

void Helium::Components::ComponentPtrBase::Unlink()
{
    // If we are the head node in the component ptr registry, we need to point it to the new head
    if (m_ComponentPtrRegistryHeadIndex != Helium::Invalid<uint32_t>())
    {
        HELIUM_ASSERT(!m_Previous);

        // Assign new head node
        g_ComponentPtrRegistry[m_ComponentPtrRegistryHeadIndex] = m_Next;

        // If this new node is legit, mark it as a head node
        if (m_Next)
        {
            m_Next->m_ComponentPtrRegistryHeadIndex = m_ComponentPtrRegistryHeadIndex;
        }

        // Unmark ourself as a head node
        m_ComponentPtrRegistryHeadIndex = Helium::Invalid<uint32_t>();
    } 
    // Unlink ourself from doubly linked list of component ptrs
    else if (m_Previous)
    {
        m_Previous->m_Next = m_Next;
    }

    if (m_Next)
    {
        m_Next->m_Previous = m_Previous;
    }

    m_Previous = 0;
    m_Next = 0;
}
