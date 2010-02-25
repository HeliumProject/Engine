#include "stdafx.h"
#include "ElementSetSerializer.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS(ElementSetSerializer);

ElementSetSerializer::ElementSetSerializer()
{

}

ElementSetSerializer::~ElementSetSerializer()
{

}

void ElementSetSerializer::ConnectData(Nocturnal::HybridPtr<void> data)
{
  __super::ConnectData( data );

  m_Data.Connect( Nocturnal::HybridPtr<DataType> (data.Address(), data.State()) );
}

size_t ElementSetSerializer::GetSize() const
{
  return m_Data->size();
}

void ElementSetSerializer::Clear()
{
  return m_Data->clear();
}

bool ElementSetSerializer::Set(const Serializer* src, u32 flags)
{
  const ElementSetSerializer* rhs = ConstObjectCast<ElementSetSerializer>(src);
  if (!rhs)
  {
    return false;
  }

  int index = 0;

  DataType::const_iterator itr = rhs->m_Data->begin();
  DataType::const_iterator end = rhs->m_Data->end();
  for ( ; itr != end; ++itr )
  {
    if (flags & SerializerFlags::Shallow)
    {
      m_Data->insert(*itr);
    }
    else
    {
      m_Data->insert((*itr)->Clone());
    }
  }

  return true;
}

bool ElementSetSerializer::Equals(const Serializer* s) const
{
  const ElementSetSerializer* rhs = ConstObjectCast<ElementSetSerializer>(s);
  if (!rhs)
  {
    return false;
  }

  if (m_Data->size() != rhs->m_Data->size())
  {
    return false;
  }

  DataType::const_iterator itrLHS = m_Data->begin();
  DataType::const_iterator endLHS = m_Data->end();
  DataType::const_iterator itrRHS = rhs->m_Data->begin();
  DataType::const_iterator endRHS = rhs->m_Data->end();
  for ( ; itrLHS != endLHS && itrRHS != endRHS; ++itrLHS, ++itrRHS )
  {
    if ((*itrLHS) == (*itrRHS))
    {
      continue;
    }

    if (!(*itrLHS)->Equals(*itrRHS))
    {
      return false;
    }
  }

  return true;
}

void ElementSetSerializer::Serialize(Archive& archive) const
{
  V_Element components;

  DataType::const_iterator itr = m_Data->begin();
  DataType::const_iterator end = m_Data->end();
  for ( ; itr != end; ++itr )
  {
    if (!itr->ReferencesObject())
    {
      continue;
    }

    components.push_back(*itr);
  }

  archive.Serialize(components);
}

void ElementSetSerializer::Deserialize(Archive& archive)
{
  V_Element components;
  archive.Deserialize(components);

  // if we are referring to a real field, clear its contents
  m_Data->clear();

  V_Element::const_iterator itr = components.begin();
  V_Element::const_iterator end = components.end();
  for ( ; itr != end; ++itr )
  {
    m_Data->insert(*itr);
  }
}

void ElementSetSerializer::Host(Visitor& visitor)
{
  DataType::iterator itr = const_cast<Serializer::DataPtr<DataType>&>(m_Data)->begin();
  DataType::iterator end = const_cast<Serializer::DataPtr<DataType>&>(m_Data)->end();
  for ( ; itr != end; ++itr )
  {
    if (!itr->ReferencesObject())
    {
      continue;
    }

    // just a note, this code is problematic with STLPort, but i wasn't 
    // able to figure out how to fix it ... works fine with msvc native iterators
    // i wish i had saved the compile error; geoff suspects it is const-ness related
    //
    if (!visitor.VisitPointer(*itr))
    {
      continue;
    }

    (*itr)->Host( visitor );
  }
}
