#pragma once

#include "API.h"


#include "FileBackedAttribute.h"
#include "Finder/ExtensionSpecs.h"


namespace Asset
{
  class ASSET_API ArtFileAttribute : public FileBackedAttribute
  {
  private:
    static const Finder::FinderSpec& s_FileFilter;

  public:
    tuid  m_FileID;
    std::string m_FragmentNode;
    Math::Vector3 m_Extents;
    Math::Vector3 m_Offset;
    Math::Vector3 m_Minima;
    Math::Vector3 m_Maxima;

    ArtFileAttribute( tuid fileID = TUID::Null )
      : m_FileID( fileID )
    {
    }

    virtual ~ArtFileAttribute() {}
    virtual Attribute::AttributeUsage GetAttributeUsage() const NOC_OVERRIDE;

    virtual tuid GetFileID() const NOC_OVERRIDE;
    virtual void SetFileID( const tuid& fileID ) NOC_OVERRIDE;
    virtual const Finder::FinderSpec* GetFileFilter() const NOC_OVERRIDE;

    REFLECT_DECLARE_CLASS( ArtFileAttribute, FileBackedAttribute );

    static void EnumerateClass( Reflect::Compositor<ArtFileAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< ArtFileAttribute > ArtFileAttributePtr;
}