#pragma once

#include "API.h"
#include "Platform/Assert.h"
#include "Foundation/Memory/SmartPtr.h"
#include "AttributeCategoryTypes.h"

#include <map>

namespace Attribute
{
  class ATTRIBUTE_API AttributeCategory;
  typedef Nocturnal::SmartPtr< AttributeCategory > AttributeCategoryPtr;
  typedef std::map< AttributeCategoryType, AttributeCategoryPtr > M_AttributeCategories;

  class ATTRIBUTE_API AttributeBase;
  typedef Nocturnal::SmartPtr< AttributeBase > AttributePtr;

  class ATTRIBUTE_API AttributeCategories
  {
  private:
    static AttributeCategories* s_Instance;
    static i32 s_InitCount;

    M_AttributeCategories m_Categories;

  private:
    AttributeCategories();

  public:
    virtual ~AttributeCategories();

    static bool Initialize()
    {
      if (++s_InitCount == 1)
      {
        s_Instance = new AttributeCategories ();
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

    static AttributeCategories* GetInstance()
    {
      NOC_ASSERT(s_InitCount);
      return s_Instance;
    }

    const M_AttributeCategories& GetCategories() const;

    const AttributeCategoryPtr& GetCategory( AttributeCategoryType type );

    // Puts the specified attribute into the appropriate category.
    void Categorize( const AttributePtr& attribute );
  };
}
