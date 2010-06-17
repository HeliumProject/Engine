#pragma once

#include "Pipeline/API.h"

#include "Pipeline/Asset/Classes/EntityAsset.h"

#include "Pipeline/Asset/Components/FileBackedComponent.h"

namespace Asset
{
  class PIPELINE_API WorldFileComponent : public FileBackedComponent
  {
  private:
    REFLECT_DECLARE_CLASS( WorldFileComponent, FileBackedComponent );
    static void EnumerateClass( Reflect::Compositor<WorldFileComponent>& comp );

  private:
    static const Finder::FinderSpec& s_FileFilter;

  public:

    WorldFileComponent()
    {
    }

    virtual Component::ComponentUsage GetComponentUsage() const NOC_OVERRIDE;
    virtual Component::ComponentCategoryType GetCategoryType() const NOC_OVERRIDE;

    virtual const Finder::FinderSpec* GetFileFilter() const NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< WorldFileComponent > WorldFileComponentPtr;
}