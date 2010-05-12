#include "AttributeCategory.h"

using namespace Attribute;

REFLECT_DEFINE_CLASS( AttributeCategory );

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AttributeCategory::AttributeCategory( AttributeCategoryType type, u32 sortGroup, const std::string& name, const std::string& shortDesc, const std::string& longDesc )
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
AttributeCategory::~AttributeCategory()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds an attribute to the list maintained by this category.
// 
bool AttributeCategory::AddAttribute( const AttributePtr& attrib )
{
  return m_Attributes.insert( M_Attribute::value_type( attrib->GetType(), attrib ) ).second;
}