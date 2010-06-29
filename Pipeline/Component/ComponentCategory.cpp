#include "Pipeline/Component/ComponentCategory.h"

using namespace Component;

REFLECT_DEFINE_CLASS( ComponentCategory );

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ComponentCategory::ComponentCategory( ComponentCategoryType type, u32 sortGroup, const tstring& name, const tstring& shortDesc, const tstring& longDesc )
: m_Type( type )
, m_SortGroup( sortGroup )
, m_Name( name )
, m_ShortDescription( shortDesc )
, m_LongDescription( longDesc )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ComponentCategory::~ComponentCategory()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds an attribute to the list maintained by this category.
// 
bool ComponentCategory::AddComponent( const ComponentPtr& attrib )
{
  return m_Components.insert( M_Component::value_type( attrib->GetType(), attrib ) ).second;
}