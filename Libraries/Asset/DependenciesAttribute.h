#pragma once

#include "API.h"
#include "Finder/AssetSpecs.h"

#include "Attribute/Attribute.h"
#include "File/File.h"

namespace Asset
{
  class ASSET_API DependenciesAttribute : public Attribute::AttributeBase
  {
  public:

      File::S_Reference m_FileReferences;

  public:
    REFLECT_DECLARE_CLASS( DependenciesAttribute, AttributeBase );

    static void EnumerateClass( Reflect::Compositor<DependenciesAttribute>& comp );

    virtual Attribute::AttributeUsage GetAttributeUsage() const NOC_OVERRIDE { return Attribute::AttributeUsages::Class; }
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< DependenciesAttribute > DependenciesAttributePtr;
}