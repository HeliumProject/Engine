#pragma once

#include "API.h"
#include "AssetClass.h"
#include "Attribute/AttributeCategoryTypes.h" 

#include "File/Reference.h"

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
        : m_FileReference( NULL )
    {
    }
    virtual ~FileBackedAttribute() {}
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;
    
    virtual File::Reference& GetFileReference() const;
    virtual void SetFileReference( const File::Reference& fileReference );

    virtual const Finder::FinderSpec* GetFileFilter() const = 0;

    REFLECT_DECLARE_ABSTRACT( FileBackedAttribute, AttributeBase );

  protected:
      File::ReferencePtr m_FileReference;
  };

  typedef Nocturnal::SmartPtr< FileBackedAttribute > FileBackedAttributePtr;
}