#pragma once

#include "Pipeline/API.h" 
#include "Platform/Assert.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Component.h"
#include "ComponentCategory.h"
#include "ComponentCategoryTypes.h"

#include <map>

namespace Component
{

  class PIPELINE_API ComponentCategories
  {
  private:
    static ComponentCategories* s_Instance;
    static i32 s_InitCount;

    M_ComponentCategories m_Categories;

  private:
    ComponentCategories();

  public:
    virtual ~ComponentCategories();

    static bool Initialize()
    {
      if (++s_InitCount == 1)
      {
        s_Instance = new ComponentCategories ();
      }
      else
      {
        NOC_BREAK(); 
      }

      return s_Instance != NULL;
    }

    static void Cleanup()
    {
      if (--s_InitCount == 0)
      {
        delete s_Instance;
        s_Instance = NULL;
      }
    }

    static ComponentCategories* GetInstance()
    {
      NOC_ASSERT(s_InitCount);
      return s_Instance;
    }

    const M_ComponentCategories& GetCategories() const;

    const ComponentCategoryPtr& GetCategory( ComponentCategoryType type );

    // Puts the specified attribute into the appropriate category.
    void Categorize( const ComponentPtr& attribute );
  };
}
