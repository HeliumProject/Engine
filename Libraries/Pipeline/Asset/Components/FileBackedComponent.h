#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Component/ComponentCategoryTypes.h"

namespace Finder
{
  class FinderSpec;
}

namespace Asset
{
  class PIPELINE_API FileBackedComponent NOC_ABSTRACT: public Component::ComponentBase
  {
  public:
    FileBackedComponent()
    {
    }
    virtual ~FileBackedComponent() {}
    virtual Component::ComponentCategoryType GetCategoryType() const NOC_OVERRIDE;
    
    virtual const Nocturnal::Path& GetPath() const;
    virtual void SetPath( const Nocturnal::Path& path );

    virtual const std::string& GetFileFilter() const = 0;

    REFLECT_DECLARE_ABSTRACT( FileBackedComponent, ComponentBase );

  protected:
      Nocturnal::Path m_Path;
  };

  typedef Nocturnal::SmartPtr< FileBackedComponent > FileBackedComponentPtr;
}