#pragma once

#include "Pipeline/API.h"
#include "AssetClass.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Serializers.h"
#include "Foundation/TUID.h"

namespace Asset
{
  /////////////////////////////////////////////////////////////////////////////
  class AssetFile;
  typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
  typedef std::vector< AssetFilePtr > V_AssetFiles;
  typedef std::map< u64, AssetFilePtr > M_AssetFiles;
  typedef Nocturnal::OrderedSet< AssetFilePtr > OS_AssetFiles;
  
  class PIPELINE_API AssetFile : public Reflect::Element
  {   
  public:
    AssetFile();
    AssetFile( Nocturnal::Path& path );
    virtual ~AssetFile();

    static AssetFilePtr CreateAssetFile( const std::string& path );

  public:
    std::string GetFilePath()
    {
        return m_Path.Get();
    }

    void SetPath( Nocturnal::Path& path )
    {
        m_Path = path;
    }
    const Nocturnal::Path& GetPath()
    {
        return m_Path;
    }

    const std::string& GetShortName();
    const std::string& GetFileFilter();
    const std::string& GetExtension();
    const std::string& GetFileType();
    
    u64 GetSize();
    const std::string& GetRCSUser();

    AssetType GetAssetType();

    static AssetClassPtr GetAssetClass( AssetFile* assetFile );

    void AddAttribute( const std::string& attrName, const std::string& attrValue, bool canAppend = true );
    const std::map< std::string, std::string >& GetComponents() const { return m_Attributes; }

    void AddDependency( const Nocturnal::Path& path );
    void SetDependencies( const std::set< Nocturnal::Path >& dependencies );
    bool HasDependencies() { return !m_Dependencies.empty(); }
    const std::set< Nocturnal::Path >& GetDependencies() const { return m_Dependencies; }
    void GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, std::set< Nocturnal::Path >& dependencies );

    void SetRowID( const u64 rowID ) { m_RowID = rowID; }
    u64 GetRowID() { return m_RowID; }

 public:
     REFLECT_DECLARE_ABSTRACT(AssetFile, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<AssetFile>& comp );

  private:
    void Init();
    void GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, std::set< Nocturnal::Path >& dependencies, std::set< Nocturnal::Path >& visited, u32 depth );

  private:
      Nocturnal::Path m_Path;

    std::string   m_ShortName;
    std::string   m_FileFilter;
    std::string   m_Extension;
    std::string   m_FileType;
    AssetType     m_AssetType;
    u64           m_Size;
    std::map< std::string, std::string >      m_Attributes;
    std::set< Nocturnal::Path > m_Dependencies;
    u64           m_RowID;

    friend class CacheDB;
  };
}
