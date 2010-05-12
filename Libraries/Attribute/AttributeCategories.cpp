#include "AttributeCategories.h"
#include "AttributeCategory.h"
#include "Exceptions.h"

using namespace Attribute;

AttributeCategories* AttributeCategories::s_Instance = NULL;
i32 AttributeCategories::s_InitCount = 0;

AttributeCategories::AttributeCategories()
{
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Animation,      new AttributeCategory( AttributeCategoryTypes::Animation,     1, "Animation", "Attributes for animating.", "" ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Cinematic,      new AttributeCategory( AttributeCategoryTypes::Cinematic,     0, "Cinematic", "Cinematic attributes.", "" ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::File,           new AttributeCategory( AttributeCategoryTypes::File,          1, "File", "Attributes that reference another file.", "File attributes might reference a Maya file, texture file, another entity file, etc." ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Font,           new AttributeCategory( AttributeCategoryTypes::Font,          1, "Font", "Font Attributes", "These attributes are used for describing the in-game font." ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Lighting,       new AttributeCategory( AttributeCategoryTypes::Lighting,      1, "Lighting", "Attributes for creating lit objects.", "" ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::LOD,            new AttributeCategory( AttributeCategoryTypes::LOD,           1, "LOD", "Level of detail.", "Attributes for setting up level of detail on objects." ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Meta,           new AttributeCategory( AttributeCategoryTypes::Meta,          1, "MetaInfo", "Attributes with extra information for the tools.", "These attributes contain information that is not used by the game.  Instead, they contain information that can be displayed within the tools such as showing a screenshot of an entity." ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Misc,           new AttributeCategory( AttributeCategoryTypes::Misc,          1, "Misc.", "Miscellaneous attributes that don't fit in other categories.", "" ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Gameplay,       new AttributeCategory( AttributeCategoryTypes::Gameplay,      1, "Gameplay", "Attributes for setting up gameplay.", "" ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Configuration,  new AttributeCategory( AttributeCategoryTypes::Configuration, 1, "Configuration", "Attributes for setting/controlling configuration values.", "" ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::PostEffects,    new AttributeCategory( AttributeCategoryTypes::PostEffects,   1, "Post Effects", "Post-processing effects.", "" ) ) );
  m_Categories.insert( M_AttributeCategories::value_type( AttributeCategoryTypes::Joint,          new AttributeCategory( AttributeCategoryTypes::Joint,         1, "Joint", "Joint data.", "" ) ) );
  // TODO: validation, make sure there are unique names, build additonal maps and sets for faster lookup!
}

AttributeCategories::~AttributeCategories()
{

}

const M_AttributeCategories& AttributeCategories::GetCategories() const
{
  return m_Categories;
}

const AttributeCategoryPtr& AttributeCategories::GetCategory( AttributeCategoryType type )
{
  M_AttributeCategories::const_iterator found = m_Categories.find( type );
  
  if ( found == m_Categories.end() )
  {
    throw Nocturnal::Exception( "There is no category with that type" );
  }

  return found->second;
}

void AttributeCategories::Categorize( const AttributePtr& attribute )
{
  M_AttributeCategories::iterator found = m_Categories.find( attribute->GetCategoryType() );
  if ( found != m_Categories.end() )
  {
    AttributeCategoryPtr& category = found->second;
    category->AddAttribute( attribute );
  }
  else
  {
    throw Nocturnal::Exception( "Attribute '%s' could not be categorized.", attribute->GetClass()->m_ShortName.c_str() );
  }
}