#pragma once

#include "API.h"
#include "AttributeCategoryTypes.h"
#include "AttributeCollection.h"

namespace Attribute
{
  class ATTRIBUTE_API AttributeCategory : public Reflect::Element
  {
  private:
    AttributeCategoryType m_Type;
    u32                   m_SortGroup;
    std::string           m_Name;
    std::string           m_ShortDescription;
    std::string           m_LongDescription;
    M_Attribute           m_Attributes;

    REFLECT_DECLARE_CLASS( AttributeCategory, Reflect::Element );

  private:
    // Private constructor required for reflect
    AttributeCategory() {}

  public:
    AttributeCategory( AttributeCategoryType type, u32 sortGroup, const std::string& name, const std::string& shortDesc, const std::string& longDesc );
    virtual ~AttributeCategory();

    inline AttributeCategoryType Type() const
    {
      return m_Type;
    }

    inline u32 SortGroup() const
    {
      return m_SortGroup;
    }

    inline const std::string& Name() const
    {
      return m_Name;
    }

    inline const std::string& ShortDescription() const
    {
      return m_ShortDescription;
    }

    inline const std::string& LongDescription() const
    {
      return m_LongDescription;
    }

    inline const M_Attribute& Attributes() const
    {
      return m_Attributes;
    }

    bool AddAttribute( const AttributePtr& attrib );
  };

}
