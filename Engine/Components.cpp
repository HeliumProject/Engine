
#include "EnginePch.h"
#include "Engine/Components.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Components;
using namespace Helium::Components::Private;

REFLECT_DEFINE_ABSTRACT(Component);

void Component::AcceptCompositeVisitor( Reflect::Composite& comp )
{
  Reflect::Field* fieldHandle = comp.AddField(&Component::m_Handle, TXT("m_Handle"));
}

const static TypeId MAX_TYPE_ID = 0xFFFF - 1;

////////////////////////////////////////////////////////////////////////
//       System Implementation
////////////////////////////////////////////////////////////////////////

// TypeId indexes into this
Private::V_ComponentTypes         Private::g_ComponentTypes;

TypeId Components::RegisterType( const Reflect::Class *_class, TypeData &_type_data, TypeData *_base_type_data, uint16_t _count )
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
  g_ComponentTypes.push_back(component_type_temp);

  // Cache a reference to the type
  ComponentType &component_type = g_ComponentTypes.back();

  // Set type id on static member of the class so we can easily get this data later
  TypeId type_id = g_ComponentTypes.size() - 1;

  // Assert that we havn't already registered this type and then set up the data
  HELIUM_ASSERT(_type_data.TypeId == NULL_TYPE_ID);
  _type_data.TypeId = type_id;

  // Update bookkeeping fields
  component_type.m_Class = _class;
  component_type.m_FirstUnallocatedIndex = 0;
  component_type.m_Count = _count;

  component_type.m_Roster.resize(_count);
  component_type.m_Instances.resize(_count);
  
  // If we have a parent
  if (_base_type_data)
  {
    std::vector<TypeId> base_implemented_types = g_ComponentTypes[_base_type_data->TypeId].m_ImplementedTypes;

    // Add base to implemented types, and ourselves to base's implementing types
    component_type.m_ImplementedTypes.push_back(_base_type_data->TypeId);
    g_ComponentTypes[_base_type_data->TypeId].m_ImplementingTypes.push_back(type_id);

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
    i < component_type.m_Count; ++i)
  {
    component_type.m_Roster[i] = i;
    component_type.m_Instances[i].Component = Reflect::AssertCast<Component>(_class->m_Creator());
    component_type.m_Instances[i].PreviousHandle = NULL_HANDLE;
    component_type.m_Instances[i].NextHandle = NULL_HANDLE;
    component_type.m_Instances[i].RosterIndex = i;
    component_type.m_Instances[i].Generation = 0;
  }

  // Return the component type's id
  HELIUM_ASSERT(type_id <= MAX_TYPE_ID)
  return type_id;
}

Component* Components::ResolveHandle( Handle _handle )
{
  ComplexHandle handle;
  handle.Whole = _handle;

  ComponentInstance &instance = g_ComponentTypes[handle.Part.TypeId].m_Instances[handle.Part.Index];

  if (instance.Generation == handle.Part.Generation)
  {
    return instance.Component;
  }
  
  return NULL;
}

Component* Components::Allocate(HostContext &_host, TypeId _type, void *_init_data)
{
  // Make sure type id is good
  HELIUM_ASSERT(_type < g_ComponentTypes.size());

  // Do we have a free component to allocate?
  if (g_ComponentTypes[_type].m_FirstUnallocatedIndex >= g_ComponentTypes[_type].m_Count)
  {
    // Could not allocate the component because we ran out..
    HELIUM_ASSERT_MSG(false, TXT("Could not allocate component of type %d for host %d"), _type, _host);
    return 0;
  }

  // Find out where the component we should allocate is in the roster
  uint16_t roster_index = g_ComponentTypes[_type].m_FirstUnallocatedIndex++;
  uint16_t component_index = g_ComponentTypes[_type].m_Roster[roster_index];

  // Get the handle and pointer
  ComplexHandle handle;
  handle.Part.TypeId = _type;
  handle.Part.Index = component_index;
  handle.Part.Generation = 0;

  // Insert into chain
  InsertIntoChain(handle.Whole, _host.FirstHandle);
  _host.FirstHandle = handle.Whole;

  // Return pointer
  ComponentInstance &instance = ResolveComponentInstance(handle.Whole);

  // Component is now attached
  instance.Component->SetHandle(handle.Whole);
  _host.OnAttach(*instance.Component, _init_data);
  // Should we call back into component here to let it do stuff?
  // Not going to as we have this callback on the host, and the host
  // will pass itself

  return instance.Component;
}

void Components::Free( HostContext &_host, Component &_component )
{
  ComplexHandle handle;
  handle.Whole = _component.GetHandle();

  // Component is already freed or component doesn't have a good handle for some reason
  HELIUM_ASSERT(handle.Whole != NULL_HANDLE);

  // Cache to save typing
  ComponentType &component_type = g_ComponentTypes[handle.Part.TypeId];
  ComponentInstance &freed_instance = ResolveComponentInstance(handle.Whole);

  _host.OnDetach(_component);
  _component.SetHandle(NULL_HANDLE);

  // Remove from the component chain (and special case to fix the first handle on host if this is first component in chain
  if (_host.FirstHandle == handle.Whole)
  {
    _host.FirstHandle = freed_instance.NextHandle;
  }
  RemoveFromChain(handle.Whole);

  // Increment generation to invalidate old handles
  ++freed_instance.Generation;

  // Get roster indices we will manipulate
  uint16_t used_roster_index = freed_instance.RosterIndex;
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
    component_type.m_Roster[used_roster_index] = component_type.m_Roster[freed_roster_index];
    component_type.m_Roster[freed_roster_index] = handle.Part.Index;

    // Swap the roster index of the highest in-use component and the recently freed component
    component_type.m_Instances[component_type.m_Roster[used_roster_index]].RosterIndex = used_roster_index;
    component_type.m_Instances[component_type.m_Roster[freed_roster_index]].RosterIndex = freed_roster_index;
  }
}

void Components::FreeAll( HostContext &_host )
{
  Handle handle = _host.FirstHandle;
  while (handle != NULL_HANDLE)
  {
    ComponentInstance &ci = ResolveComponentInstance(handle);
    handle = ci.NextHandle;

    Free(_host, *ci.Component);
  }
}

Component* Components::FindOneComponent( HostContext &_host, TypeId _type )
{
  return InternalFindOneComponent(_host, _type, false);
}

Component* Components::FindOneComponentThatImplements( HostContext &_host, TypeId _type )
{
  return InternalFindOneComponent(_host, _type, true);
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

void Components::Private::InsertIntoChain( Handle _handle, Handle _insertion_point )
{
  // If we are inserting into a 0-length chain do nothing
  if (_insertion_point != NULL_HANDLE)
  {
    ComponentInstance &instance = ResolveComponentInstance(_handle);
    ComponentInstance &next_instance = ResolveComponentInstance(_insertion_point);

    // Fix inserted node's next/previous pointers
    instance.NextHandle = _insertion_point;
    instance.PreviousHandle = next_instance.PreviousHandle;

    // Fix previous component's next pointer
    if (next_instance.PreviousHandle != NULL_HANDLE)
    {
      ComponentInstance &previous_instance = ResolveComponentInstance(next_instance.PreviousHandle);
      previous_instance.NextHandle = _handle;
    }

    // Fix next component's previous pointer
    next_instance.PreviousHandle = _handle;
  }
}

void Components::Private::RemoveFromChain( Handle _handle )
{
  ComponentInstance &instance = ResolveComponentInstance(_handle);

  Handle next_handle = instance.NextHandle;
  Handle previous_handle = instance.PreviousHandle;

  // previous node's next pointer
  if (instance.PreviousHandle != NULL_HANDLE)
  {
    ComponentInstance &previous = ResolveComponentInstance(instance.PreviousHandle);
    previous.NextHandle = next_handle;
  }

  // next node's previous pointer
  if (instance.NextHandle != NULL_HANDLE)
  {
    ComponentInstance &next = ResolveComponentInstance(instance.NextHandle);
    next.PreviousHandle = previous_handle;
  }

  // wipe our node
  instance.PreviousHandle = NULL_HANDLE;
  instance.NextHandle = NULL_HANDLE;
}

Component* Components::Private::InternalFindOneComponent( HostContext &_host, TypeId _type_id, bool _implements )
{
  ComponentType &type = g_ComponentTypes[_type_id];

  ComplexHandle handle;
  handle.Whole = _host.FirstHandle;

  while (handle.Whole != NULL_HANDLE)
  {
    if (handle.Part.TypeId == _type_id)
    {
      return g_ComponentTypes[handle.Part.TypeId].m_Instances[handle.Part.Index].Component;
    }

    if (_implements && TypeImplementsType(handle.Part.TypeId, _type_id))
    {
      return g_ComponentTypes[handle.Part.TypeId].m_Instances[handle.Part.Index].Component;
    }

    handle.Whole = g_ComponentTypes[handle.Part.TypeId].m_Instances[handle.Part.Index].NextHandle;
  }

  return NULL;
}

////////////////////////////////////////////////////////////////////////
//       System Functions
////////////////////////////////////////////////////////////////////////

void Components::Initialize()
{
  // Sanity checks for the ComplexHandle union
  ComplexHandle complex_handle;
  HELIUM_ASSERT(sizeof(complex_handle.Whole) == sizeof(complex_handle.Part));
  HELIUM_ASSERT(sizeof(ComplexHandle) == sizeof(Handle));

  // Register base component with reflect
  Reflect::RegisterClassType<Components::Component>(TXT("Component"));
  RegisterType(Reflect::GetClass<Component>(), Component::GetStaticComponentTypeData(), 0, 0);
}

void Components::Cleanup()
{
  for (TypeId type_id = 0; type_id < g_ComponentTypes.size(); ++type_id)
  {
    V_ComponentInstances &components = g_ComponentTypes[type_id].m_Instances;
    for (V_ComponentInstances::iterator iter = components.begin(); 
      iter != components.end(); ++iter)
    {
      delete iter->Component;
    }
  }

  Reflect::UnregisterClassType<Components::Component>();
}

void FindAllComponentsOnHost(HostContext &_host, TypeId _type, IVectorWrapper &_components, bool _implements)
{
  Private::ComplexHandle handle;
  handle.Whole = _host.FirstHandle;

  ComponentInstance *instance = 0;

  while (handle.Whole != NULL_HANDLE)
  {
    instance = &ResolveComponentInstance(handle.Whole);

    if (_type == handle.Part.TypeId ||
      _implements && Components::TypeImplementsType(handle.Part.TypeId, _type))
    {
      _components.PushBack(instance->Component);
    }

    handle.Whole = instance->NextHandle;
  }
}

void Components::Private::FindAllComponents( HostContext &_host, TypeId _type, IVectorWrapper &_components )
{
  FindAllComponentsOnHost(_host, _type, _components, false);
}

void Components::Private::FindAllComponentsThatImplement( HostContext &_host, TypeId _type, IVectorWrapper &_components )
{
  FindAllComponentsOnHost(_host, _type, _components, true);
}

void Components::Private::FindAllComponents( TypeId _type, IVectorWrapper &_components )
{
  HELIUM_ASSERT(_type < g_ComponentTypes.size());
  ComponentType &component_type = g_ComponentTypes[_type];

  // Resize the vector and add components to it
  _components.Resize(component_type.m_FirstUnallocatedIndex);
  if (component_type.m_FirstUnallocatedIndex == 0)
  {
    return;
  }

  Component **components = _components.GetFirst();

  for (uint16_t index = 0; index < component_type.m_FirstUnallocatedIndex; ++index)
  {
    components[index] = component_type.m_Instances[component_type.m_Roster[index]].Component;
  }
}

void Components::Private::FindAllComponentsThatImplement( TypeId _type, IVectorWrapper &_components )
{
  HELIUM_ASSERT(_type < g_ComponentTypes.size());

  uint32_t component_count = 0;
  uint32_t current_index = 0;

  ComponentType &this_component_type = g_ComponentTypes[_type];

  // Count all the components that implement the given type
  component_count += this_component_type.m_FirstUnallocatedIndex;

  for (std::vector<TypeId>::iterator iter = g_ComponentTypes[_type].m_ImplementingTypes.begin();
    iter != g_ComponentTypes[_type].m_ImplementingTypes.end(); ++iter)
  {
    ComponentType &component_type = g_ComponentTypes[*iter];
    component_count += component_type.m_FirstUnallocatedIndex;
  }

  // Resize the vector to contain all components
  _components.Resize(component_count);
  if (component_count == 0)
  {
    return;
  }

  Component **components = _components.GetFirst();

  for (uint16_t roster_index = 0; roster_index < this_component_type.m_FirstUnallocatedIndex; ++roster_index)
  {
    components[current_index] = this_component_type.m_Instances[this_component_type.m_Roster[roster_index]].Component;
    ++current_index;
  }

  // Put components in the vector, loop over each type, then each component of that type
  for (std::vector<TypeId>::iterator iter = g_ComponentTypes[_type].m_ImplementingTypes.begin();
    iter != g_ComponentTypes[_type].m_ImplementingTypes.end(); ++iter)
  {  
    ComponentType &component_type = g_ComponentTypes[*iter];
    for (uint16_t roster_index = 0; roster_index < component_type.m_FirstUnallocatedIndex; ++roster_index)
    {
      components[current_index] = component_type.m_Instances[component_type.m_Roster[roster_index]].Component;
      ++current_index;
    }
  }
}

Private::ComponentInstance & Components::Private::ResolveComponentInstance( Handle _handle )
{
  Private::ComplexHandle handle;
  handle.Whole = _handle;

  return g_ComponentTypes[handle.Part.TypeId].m_Instances[handle.Part.Index];
}