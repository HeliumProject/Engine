#pragma once

#include "API.h"
#include "AssetClass.h"
#include "Attribute/AttributeCategoryTypes.h" 

namespace Finder
{
  class FinderSpec;
}

namespace Asset
{
  class ASSET_API FileBackedAttribute NOC_ABSTRACT: public Attribute::AttributeBase
  {
  public:
    FileBackedAttribute()
    {
    }
    virtual ~FileBackedAttribute() {}
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;
    
    virtual const Nocturnal::Path& GetPath() const;
    virtual void SetPath( const Nocturnal::Path& path );

    virtual const Finder::FinderSpec* GetFileFilter() const = 0;

    REFLECT_DECLARE_ABSTRACT( FileBackedAttribute, AttributeBase );

  protected:
      Nocturnal::Path m_Path;
  };

  typedef Nocturnal::SmartPtr< FileBackedAttribute > FileBackedAttributePtr;
}