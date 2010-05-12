#pragma once

#include "API.h"



#include "EntityAsset.h"

#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"

#include "FileBackedAttribute.h"

namespace Asset
{
  class ASSET_API WorldFileAttribute : public FileBackedAttribute
  {
  private:
    REFLECT_DECLARE_CLASS( WorldFileAttribute, FileBackedAttribute );
    static void EnumerateClass( Reflect::Compositor<WorldFileAttribute>& comp );

  private:
    static const Finder::FinderSpec& s_FileFilter;

  public:
    tuid  m_FileID;

    WorldFileAttribute( tuid fileID = TUID::Null )
      : m_FileID( fileID )
    {
    }

    virtual Attribute::AttributeUsage GetAttributeUsage() const NOC_OVERRIDE;
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;

    virtual tuid GetFileID() const NOC_OVERRIDE;
    virtual void SetFileID( const tuid& fileID ) NOC_OVERRIDE;
    virtual const Finder::FinderSpec* GetFileFilter() const NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< WorldFileAttribute > WorldFileAttributePtr;
}