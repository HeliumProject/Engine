#pragma once

#include "Pipeline/API.h"

#include "Pipeline/Asset/Classes/EntityAsset.h"

#include "Pipeline/Asset/Attributes/FileBackedAttribute.h"

namespace Asset
{
  class PIPELINE_API WorldFileAttribute : public FileBackedAttribute
  {
  private:
    REFLECT_DECLARE_CLASS( WorldFileAttribute, FileBackedAttribute );
    static void EnumerateClass( Reflect::Compositor<WorldFileAttribute>& comp );

  private:
    static const Finder::FinderSpec& s_FileFilter;

  public:

    WorldFileAttribute()
    {
    }

    virtual Attribute::AttributeUsage GetAttributeUsage() const NOC_OVERRIDE;
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;

    virtual const Finder::FinderSpec* GetFileFilter() const NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< WorldFileAttribute > WorldFileAttributePtr;
}