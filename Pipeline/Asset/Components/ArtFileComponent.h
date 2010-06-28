#pragma once

#include "Pipeline/API.h"


#include "FileBackedComponent.h"

namespace Asset
{
  class PIPELINE_API ArtFileComponent : public FileBackedComponent
  {
  private:
      static const tstring s_FileFilter;

  public:
    tstring m_FragmentNode;
    Math::Vector3 m_Extents;
    Math::Vector3 m_Offset;
    Math::Vector3 m_Minima;
    Math::Vector3 m_Maxima;

    ArtFileComponent()
    {
    }

    virtual ~ArtFileComponent() {}
    virtual Component::ComponentUsage GetComponentUsage() const NOC_OVERRIDE;

    virtual const tstring& GetFileFilter() const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( ArtFileComponent, FileBackedComponent );

    static void EnumerateClass( Reflect::Compositor<ArtFileComponent>& comp );
  };

  typedef Nocturnal::SmartPtr< ArtFileComponent > ArtFileComponentPtr;
}