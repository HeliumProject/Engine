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
    FileBackedAttribute();
    virtual ~FileBackedAttribute() {}
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;
    virtual std::string GetFilePath() const;
    virtual tuid GetFileID() const = 0;
    virtual void SetFileID( const tuid& fileID ) = 0;
    virtual const Finder::FinderSpec* GetFileFilter() const = 0;

    REFLECT_DECLARE_ABSTRACT( FileBackedAttribute, AttributeBase );
  };

  typedef Nocturnal::SmartPtr< FileBackedAttribute > FileBackedAttributePtr;
}