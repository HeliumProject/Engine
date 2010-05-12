#pragma once

#include "API.h"
#include "AssetClass.h"

#include "Common/Container/OrderedSet.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/ManagedFile.h"
#include "Finder/Finder.h"
#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"
#include "TUID/TUID.h"

namespace Asset
{
  /////////////////////////////////////////////////////////////////////////////
  class AssetFile;
  typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
  typedef std::vector< AssetFilePtr > V_AssetFiles;
  typedef std::map< tuid, AssetFilePtr > M_AssetFiles;
  typedef Nocturnal::OrderedSet< AssetFilePtr > OS_AssetFiles;
  
  class ASSET_API AssetFile : public File::ManagedFile
  {   
  public:
    AssetFile();
    AssetFile( const File::ManagedFile* file );
    virtual ~AssetFile();

    static AssetFilePtr FindAssetFile( const tuid id, bool useCacheDB = true );
    static AssetFilePtr FindAssetFile( const std::string& path, bool useCacheDB = true );

  public:
    const tuid& GetFileID() const { return m_Id; }
    const std::string& GetFilePath() const { return m_Path; }

    const std::string& GetShortName();
    const Finder::ModifierSpec* GetModifierSpec();
    const std::string& GetExtension();
    const std::string& GetFileType();
    
    EngineType GetEngineType();
    const std::string& GetEngineTypeName();

    u64 GetSize();
    const std::string& GetP4User();
    i32 GetP4Revision();

    static AssetClassPtr GetAssetClass( const AssetFile* assetFile );

    void AddAttribute( const std::string& attrName, const std::string& attrValue, bool canAppend = true );
    const M_string& GetAttributes() const { return m_Attributes; }

    void AddDependencyID( const tuid dependencyID );
    void SetDependencyIDs( const S_tuid& dependencyIDs );
    bool HasDependencies() { return m_DependencyIDs.empty() ? false : true; }
    const S_tuid& GetDependencyIDs() const { return m_DependencyIDs; }
    void GetDependencyIDsOfType( M_AssetFiles* assetFiles, i32 type, S_tuid& dependencies );

    void SetRowID( const u64 rowID ) { m_RowID = rowID; }
    u64 GetRowID() { return m_RowID; }

 public:
    REFLECT_DECLARE_ABSTRACT(AssetFile, File::ManagedFile);
    static void EnumerateClass( Reflect::Compositor<AssetFile>& comp );

  private:
    void Init();
    void SetManagedFile( const File::ManagedFile* file );
    void GetDependencyIDsOfType( M_AssetFiles* assetFiles, i32 type, S_tuid& dependencies, S_tuid& visitedTuids, u32 depth );

  private:
    std::string   m_ShortName;
    const Finder::ModifierSpec* m_ModifierSpec;
    std::string   m_Extension;
    std::string   m_FileType;
    EngineType    m_EngineType;
    std::string   m_EngineTypeName;
    u64           m_Size;
    std::string   m_P4User;
    M_string      m_Attributes;
    S_tuid        m_DependencyIDs;
    u64           m_RowID;
    i32           m_P4LocalRevision;
    bool          m_IsTemporary;

    friend class CacheDB;
  };
}
