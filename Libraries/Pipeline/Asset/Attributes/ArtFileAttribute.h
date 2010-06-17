#pragma once

#include "Pipeline/API.h"


#include "FileBackedAttribute.h"

namespace Asset
{
  class PIPELINE_API ArtFileAttribute : public FileBackedAttribute
  {
  private:
    static const Finder::FinderSpec& s_FileFilter;

  public:
    std::string m_FragmentNode;
    Math::Vector3 m_Extents;
    Math::Vector3 m_Offset;
    Math::Vector3 m_Minima;
    Math::Vector3 m_Maxima;

    ArtFileAttribute()
    {
    }

    virtual ~ArtFileAttribute() {}
    virtual Attribute::AttributeUsage GetAttributeUsage() const NOC_OVERRIDE;

    virtual const Finder::FinderSpec* GetFileFilter() const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( ArtFileAttribute, FileBackedAttribute );

    static void EnumerateClass( Reflect::Compositor<ArtFileAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< ArtFileAttribute > ArtFileAttributePtr;
}