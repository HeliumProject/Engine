#pragma once

#include "Pipeline/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Serializers.h"

namespace Asset
{
  class PIPELINE_API AssetFolder : public Reflect::Element
  {
  public:
      Nocturnal::Path m_Directory;
      Nocturnal::Path m_FullPath;

  public:
    AssetFolder( const std::string& fullPath );

    inline const std::string& AssetFolder::GetFullPath() const
    {
      return m_FullPath;
    }

    inline const std::string& AssetFolder::GetShortName() const
    {
      return m_Directory;
    }

 public:
    REFLECT_DECLARE_ABSTRACT(AssetFolder, Reflect::Element);
    static void EnumerateClass( Reflect::Compositor<AssetFolder>& comp );
  };
  typedef Nocturnal::SmartPtr< AssetFolder > AssetFolderPtr;
  typedef std::vector< AssetFolderPtr > V_AssetFolders;
}
