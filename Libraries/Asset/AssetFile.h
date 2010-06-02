#pragma once

#include "API.h"
#include "AssetClass.h"
#include "CacheDB.h"

#include "Common/Container/OrderedSet.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Reference.h"
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
  typedef std::map< u64, AssetFilePtr > M_AssetFiles;
  typedef Nocturnal::OrderedSet< AssetFilePtr > OS_AssetFiles;
  
  class ASSET_API AssetFile : public Reflect::Element
  {   
  public:
    AssetFile();
    AssetFile( File::Reference& file );
    virtual ~AssetFile();

    static AssetFilePtr FindAssetFile( const std::string& path, CacheDB* cache = NULL );

  public:
    std::string GetFilePath()
    {
        m_FileReference->Resolve();
        return m_FileReference->GetPath();
    }

    void SetFileReference( File::Reference& fileRef )
    {
        if ( m_FileReference )
        {
            delete m_FileReference;
        }

        m_FileReference = new File::Reference( fileRef );
    }
    File::ReferencePtr& GetFileReference()
    {
        return m_FileReference;
    }

    const std::string& GetShortName();
    const Finder::ModifierSpec* GetModifierSpec();
    const std::string& GetExtension();
    const std::string& GetFileType();
    
    u64 GetSize();
    const std::string& GetRCSUser();

    AssetType GetAssetType();

    static AssetClassPtr GetAssetClass( AssetFile* assetFile );

    void AddAttribute( const std::string& attrName, const std::string& attrValue, bool canAppend = true );
    const M_string& GetAttributes() const { return m_Attributes; }

    void AddDependency( const File::ReferencePtr& fileRef );
    void SetDependencies( const File::S_Reference& dependencies );
    bool HasDependencies() { return !m_Dependencies.empty(); }
    const File::S_Reference& GetDependencies() const { return m_Dependencies; }
    void GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, File::S_Reference& dependencies );

    void SetRowID( const u64 rowID ) { m_RowID = rowID; }
    u64 GetRowID() { return m_RowID; }

 public:
     REFLECT_DECLARE_ABSTRACT(AssetFile, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<AssetFile>& comp );

  private:
    void Init();
    void GetDependenciesOfType( M_AssetFiles* assetFiles, i32 type, File::S_Reference& dependencies, File::S_Reference& visited, u32 depth );

  private:
    File::ReferencePtr m_FileReference;

    std::string   m_ShortName;
    const Finder::ModifierSpec* m_ModifierSpec;
    std::string   m_Extension;
    std::string   m_FileType;
    AssetType     m_AssetType;
    u64           m_Size;
    M_string      m_Attributes;
    File::S_Reference m_Dependencies;
    u64           m_RowID;

    friend class CacheDB;
  };
}
