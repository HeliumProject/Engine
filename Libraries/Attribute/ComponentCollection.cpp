#include "ComponentCollection.h"

#include "Common/Container/Insert.h" 

using Nocturnal::Insert; 
using namespace Reflect;
using namespace Attribute;

REFLECT_DEFINE_CLASS(ComponentCollection);

void ComponentCollection::EnumerateClass( Reflect::Compositor<ComponentCollection>& comp )
{
  comp.AddField( &ComponentCollection::m_Components, "m_Components" );
}

bool ComponentCollection::ProcessComponent( ElementPtr element, const std::string& fieldName )
{
  //
  // Legacy Component Insertion
  //  Since m_Component is in the RTTI map, any object nested below <m_Component> will be set automagically
  //  This will set element pointers of objects above the m_Component element which do not map to field variables
  //

  // we don't support enum-based primitives because the RTTI data may be GONE
  if ( element->HasType( Reflect::GetType<EnumerationSerializer>() ) )
  {
    return false;
  }

  char buf[32];
  int count = 1;

  // start w/ a key value of the field name
  std::string key = fieldName;

  // if that fails
  if (key.empty())
  {
    // try the short name name next
    key = element->GetClass()->m_ShortName;
  }

  // try our key first
  std::string hashed = key;

  // loop until we find a unqiue component name
  while (GetComponent(hashed.c_str()) != NULL)
  {
    // next index name
    sprintf(buf, "%s%d", key.c_str(), count++);

    // set the key
    hashed = buf;
  }

  // set the component
  return SetComponent(hashed, element);
}

ElementPtr ComponentCollection::GetComponent( const std::string& name, int typeID ) const
{
  if (typeID == Reflect::ReservedTypes::Any)
  {
    typeID = Reflect::GetType<Element>();
  }

  M_Element::const_iterator i = m_Components.find(name);

  if (i != m_Components.end() && i->second->HasType(typeID))
  {
    return i->second;
  }

  return NULL;
}

bool ComponentCollection::SetComponent( const std::string& name, ElementPtr element )
{
  M_Element::iterator found = m_Components.find(name);

  if (found != m_Components.end())
  {
    found->second = element;
    return true;
  }
  else
  {
    Insert<M_Element>::Result result = m_Components.insert(M_Element::value_type (name, element));

    NOC_ASSERT(result.second);
    return result.second; 
  }
}

ElementPtr ComponentCollection::RemoveComponent( const std::string& name )
{
  M_Element::iterator i = m_Components.find(std::string (name));

  if (i != m_Components.end())
  {
    ElementPtr e = i->second;

    m_Components.erase(i);

    return e;
  }

  return NULL;
}
