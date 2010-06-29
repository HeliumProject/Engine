#pragma once

#include "Pipeline/API.h"
#include "Component.h"
#include "ComponentCategoryTypes.h"

#include "Foundation/Reflect/Element.h"

namespace Component
{
  class PIPELINE_API ComponentCategory : public Reflect::Element
  {
  private:
    ComponentCategoryType m_Type;
    u32                   m_SortGroup;
    tstring           m_Name;
    tstring           m_ShortDescription;
    tstring           m_LongDescription;
    M_Component           m_Components;

    REFLECT_DECLARE_CLASS( ComponentCategory, Reflect::Element );

  private:
    // Private constructor required for reflect
    ComponentCategory() {}

  public:
    ComponentCategory( ComponentCategoryType type, u32 sortGroup, const tstring& name, const tstring& shortDesc, const tstring& longDesc );
    virtual ~ComponentCategory();

    inline ComponentCategoryType Type() const
    {
      return m_Type;
    }

    inline u32 SortGroup() const
    {
      return m_SortGroup;
    }

    inline const tstring& Name() const
    {
      return m_Name;
    }

    inline const tstring& ShortDescription() const
    {
      return m_ShortDescription;
    }

    inline const tstring& LongDescription() const
    {
      return m_LongDescription;
    }

    inline const M_Component& Components() const
    {
      return m_Components;
    }

    bool AddComponent( const ComponentPtr& attrib );
  };

  typedef Nocturnal::SmartPtr< ComponentCategory > ComponentCategoryPtr;
  typedef std::map< ComponentCategoryType, ComponentCategoryPtr > M_ComponentCategories;

}
