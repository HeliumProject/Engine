#include "Attribute.h"
#include "AttributeCollection.h"
#include "AttributeCategory.h"
#include "AttributeCategories.h"

using namespace Attribute;

REFLECT_DEFINE_ABSTRACT(AttributeBase)

void AttributeBase::EnumerateClass( Reflect::Compositor<AttributeBase>& comp )
{
  Reflect::Field* fieldEnabled = comp.AddField( &AttributeBase::m_Enabled, "m_Enabled" );
}

AttributeBase::AttributeBase()
  : m_Collection( NULL )
  , m_Enabled( true )
{

}

AttributeBase::~AttributeBase()
{

}

const AttributeCategoryPtr& AttributeBase::GetCategory() const
{
  return AttributeCategories::GetInstance()->GetCategory( GetCategoryType() );
}

AttributeCollection* AttributeBase::GetCollection() const
{
  return m_Collection;
}

void AttributeBase::SetCollection(AttributeCollection* collection)
{
  if ( m_Collection && collection && m_Collection != collection )
  {
    throw Exception ( "Cannot add attribute '%s' to collection '%s', it is already the member of another collection '%s'", GetClass()->m_ShortName.c_str(), collection->GetClass()->m_ShortName.c_str(), m_Collection->GetClass()->m_ShortName.c_str() );
  }

  m_Collection = collection;
}

bool AttributeBase::ValidateSibling( const AttributeBase* attribute, std::string& error ) const
{
  return true;
}
