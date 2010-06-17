#pragma once

#include "Pipeline/API.h"
#include "Foundation/File/Path.h"

#include "Attribute/Attribute.h"

namespace Asset
{
  class PIPELINE_API DependenciesAttribute : public Attribute::AttributeBase
  {
  public:

      Nocturnal::S_Path m_Paths;

  public:
    REFLECT_DECLARE_CLASS( DependenciesAttribute, AttributeBase );

    static void EnumerateClass( Reflect::Compositor<DependenciesAttribute>& comp );

    virtual Attribute::AttributeUsage GetAttributeUsage() const NOC_OVERRIDE { return Attribute::AttributeUsages::Class; }
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< DependenciesAttribute > DependenciesAttributePtr;
}