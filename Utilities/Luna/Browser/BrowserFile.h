#pragma once

#include "API.h"

#include "File/ManagedFile.h"

#include "TUID/TUID.h"


#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "Finder/Finder.h"

// Forwards
namespace Asset
{
  class AssetClass;
  typedef Nocturnal::SmartPtr< AssetClass > AssetClassPtr;

  class AssetFile;
  typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
  typedef std::map< tuid, AssetFilePtr > M_AssetFiles;

  class ASSETTRACKER_API AssetFile : public Nocturnal::RefCountBase< AssetFile >
  {
  public:
    AssetFile( const File::ManagedFile* file );
    virtual ~AssetFile();

    void SetManagedFile( const File::ManagedFile* file );
    void SetAssetClass( const Asset::AssetClass* assetClass );
    File::ManagedFile* GetManagedFile();

    const tuid& GetFileID() { return m_File->m_Id; }

    const std::string& GetFilePath() { return m_File->m_Path; }
    const std::string& GetRelativePath();
    const std::string& GetShortName();

    const std::string& GetP4User();

    u64 GetRowID() { return m_RowID; }
    void SetRowID( const u64 rowID ) { m_RowID = rowID; }

    const S_tuid& GetDependencyIDs() { return m_DependencyIDs; }
    void AddDependencyID( const tuid dependencyID );

    const std::string& GetEngineTypeName();

    u64 GetSize();
    const std::string& GetPrintSize();

    const Finder::ModifierSpec* GetModifierSpec();
    const std::string& GetExtension();
    const std::string& GetFileType();

    const std::string& GetScreneShotPath();
    void SetScreneShotPath( const std::string& screneShotPath ) { m_ScreneShotPath = screneShotPath; }


    const M_string& GetAttributes() const { return m_Attributes; }
    void AddAttribute( const std::string& attrName, const std::string& attrValue );

    AssetClass* GetAssetClass();

  protected:

    //
    // Members
    //

    // AssetClass (Optional)
    Asset::AssetClassPtr  m_AssetClass;

    S_tuid        m_DependencyIDs;
    std::string   m_EngineTypeName;

    M_string      m_Attributes;

    // ManagedFile
    File::ManagedFilePtr m_File;
    
    std::string   m_P4User;
    std::string   m_RelativePath;
    std::string   m_ShortName;

    const Finder::ModifierSpec* m_ModifierSpec;
    std::string   m_Extension;
    std::string   m_FileType;
    
    u64           m_Size;       // current size of the file on disk
    std::string   m_PrintSize;

    std::string   m_ArtFile;
    std::string   m_ScreneShotPath;


    // Asset DB
    u64           m_RowID;


    //
    // Helper Functions
    //

    void InitializeFileCache();
    void InitializeAssetCache();

    void PopulateStats( bool force = false );
    static void GetEngineTypeName( const Asset::AssetClass* assetClass, std::string& engineTypeName );

    const std::string& GetArtFile();
  };
}
