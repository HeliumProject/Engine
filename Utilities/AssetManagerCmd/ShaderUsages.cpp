#include "Windows/Windows.h"

#include "ShaderUsages.h"

#include "Common/InitializerStack.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/ShaderUsagesAttribute.h"
#include "Asset/EntityManifest.h"

#include "AssetManager/AssetManager.h"
#include "AssetManager/ManagedAsset.h"

#include "Attribute/Attribute.h"
#include "Attribute/AttributeHandle.h"

#include "Console/Console.h"

#include "File/ManagedFile.h"
#include "File/ManagedFileUtils.h"
#include "File/Manager.h"

#include "FileSystem/FileSystem.h"

#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"
#include "Finder/ProjectSpecs.h"

#include "Reflect/Class.h"
#include "Reflect/Serializers.h"
#include "Reflect/Version.h"
#include "Reflect/Visitor.h"

#include "Windows/Process.h"
#include "Windows/Thread.h"

#include "sqlite/src/sqlite3.h"

using namespace Finder;

using namespace AssetManager;


typedef std::map< tuid, S_tuid > M_ShaderUsages; 

///////////////////////////////////////////////////////////////////////////////
// shader_usages DB
static const char* s_InsertShaderUsageSQL = "INSERT INTO shader_usages (shader_id,asset_id) VALUES('%lld','%lld');";
static const char* s_SelectShaderUsagesSQL = "SELECT asset_id FROM shader_usages WHERE shader_id=?;";

static const char* s_DeleteShaderUsagesSQL = "DELETE FROM shader_usages WHERE shader_id='%lld' AND asset_id NOT IN (%s);";


///////////////////////////////////////////////////////////////////////////////
class ShaderUsagesDB : public SQL::SQLiteDB
{
public:
  ShaderUsagesDB( const char* friendlyName );
  virtual ~ShaderUsagesDB();

  void   InsertShaderUsages( const M_ShaderUsages& shaderUsages );
  void   SelectShaderUsages( const tuid shaderID, S_tuid& assetIds );
  void   DeleteShaderUsages( const tuid shaderID, const S_tuid& assetIds );   

protected:

  //
  // SQLiteDB overrides functions
  //

  virtual void PrepareStatements() NOC_OVERRIDE;

private:

  //
  // Members
  //

  SQL::StmtHandle   m_SelectShaderUsagesStmtHandle;
};


/////////////////////////////////////////////////////////////////////////////
// Ctor
ShaderUsagesDB::ShaderUsagesDB( const char* friendlyName )
: SQL::SQLiteDB( friendlyName )
{
}


/////////////////////////////////////////////////////////////////////////////
// Dtor
ShaderUsagesDB::~ShaderUsagesDB()
{
}




/////////////////////////////////////////////////////////////////////////////
// Prepares all of the statements.
void ShaderUsagesDB::PrepareStatements()
{
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  m_SelectShaderUsagesStmtHandle = m_DBManager->CreateStatement( s_SelectShaderUsagesSQL, "l" );
}




/////////////////////////////////////////////////////////////////////////////
// Insert a single shader usage rowrow into the ShaderUsagesDB
void ShaderUsagesDB::InsertShaderUsages( const M_ShaderUsages& shaderUsages )
{
  for each ( const M_ShaderUsages::value_type& shaderUsage in shaderUsages )
  {
    for each ( const tuid& assetId in shaderUsage.second )
    {
      if ( ( m_DBManager->ExecSQLVMPrintF( s_InsertShaderUsageSQL, shaderUsage.first, assetId ) != SQLITE_OK )
        && !m_DBManager->IsNotUniqueErr() )
      {
        throw Exception( m_DBManager->GetLastErrMsg().c_str() );
      }
    }

    DeleteShaderUsages( shaderUsage.first, shaderUsage.second );
  }
}


/////////////////////////////////////////////////////////////////////////////
// Delete shader usages that are not in the given set of assetIds,
// build up the string for checking which ids should be in the given graph
//
void ShaderUsagesDB::DeleteShaderUsages( const tuid shaderID, const S_tuid& assetIds )
{

  std::string validAssetIdsStr;
  for each ( const tuid& assetId in assetIds )
  {
    if ( !validAssetIdsStr.empty() )
    {
      validAssetIdsStr += ",";
    }

    std::stringstream idStr;
    idStr << "'" << assetId << "'";  
    validAssetIdsStr += idStr.str();
  }

  if ( m_DBManager->ExecSQLVMPrintF( s_DeleteShaderUsagesSQL, shaderID, validAssetIdsStr.c_str() ) != SQLITE_OK )
  {
    throw Exception( m_DBManager->GetLastErrMsg().c_str() );
  }
}



/////////////////////////////////////////////////////////////////////////////
void ShaderUsagesDB::SelectShaderUsages( const tuid shaderID, S_tuid& assetIds )
{
  int sqlResult = m_DBManager->ExecStatement( m_SelectShaderUsagesStmtHandle, shaderID );

  if ( sqlResult == SQLITE_DONE )
  {
    return;
  }
  else if ( sqlResult != SQLITE_ROW )
  {
    throw Exception( m_DBManager->GetLastErrMsg().c_str() );
  }
  else if ( sqlResult == SQLITE_ROW )
  {
    // Step through one or more entries, and pull the data from the columns
    tuid assetId = TUID::Null;
    while ( sqlResult == SQLITE_ROW )
    {
      assetId = TUID::Null;

      m_DBManager->GetColumnI64( m_SelectShaderUsagesStmtHandle,  0, ( i64& ) assetId );

      if ( assetId != TUID::Null )
      {
        assetIds.insert( assetId );
      }

      sqlResult = m_DBManager->StepStatement( m_SelectShaderUsagesStmtHandle );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////


// mapping of shader id to a set of assets that consume that shader
M_ShaderUsages g_ShaderUsages;

ShaderUsagesDB* g_ShaderUsagesDB = NULL;



int g_InitRef = 0;
Nocturnal::InitializerStack g_Initializers;

///////////////////////////////////////
void ShaderUsages::Initialize()
{
  if ( ++g_InitRef > 1 )
  {
    return;
  }

  Finder::Initialize();

  g_Initializers.Push( &Finder::Initialize, &Finder::Cleanup );
  g_Initializers.Push( &Reflect::Initialize, &Reflect::Cleanup );
  g_Initializers.Push( &Asset::Initialize, &Asset::Cleanup );
  g_Initializers.Push( &File::Initialize, &File::Cleanup );
  g_Initializers.Push( &AssetManager::Initialize, &AssetManager::Cleanup );

  g_ShaderUsagesDB = new ShaderUsagesDB( "AssetManagerCmd-ShaderUsagesDB" );
  g_ShaderUsagesDB->Open( 
    FinderSpecs::Project::ASSET_MANAGER_DB.GetFile( FinderSpecs::Project::ASSET_MANAGER_FOLDER ),
    FinderSpecs::Project::ASSET_MANAGER_CONFIGS.GetFolder(),
    FinderSpecs::Project::ASSET_MANAGER_DB.GetFormatVersion() );
}

///////////////////////////////////////
void ShaderUsages::Cleanup()
{
  if ( --g_InitRef )
  {
    return;
  }

  g_ShaderUsages.clear();
  delete g_ShaderUsagesDB;
  g_ShaderUsagesDB = NULL;

  g_Initializers.Cleanup();
}



///////////////////////////////////////////////////////////////////////////////
// List all of the shaders that the given asset is using
//
void ShaderUsages::ListAssetsShaders( const std::string& assetFilePath, File::V_ManagedFilePtr& shaderFiles )
{
  File::V_ManagedFilePtr assetFiles;
  File::FindManagedFiles( assetFilePath, assetFiles );

  if ( !assetFiles.empty() )
  {
    int assetIndex = File::DetermineAssetIndex( assetFiles, "list shaders for" );

    const File::ManagedFilePtr& file = assetFiles.at(assetIndex);

    Asset::AssetClassPtr assetClass = NULL;
    try
    {
      if ( ( assetClass = Asset::AssetClass::FindAssetClass( file->m_Id ) ) == NULL )
      {
        Console::Error( "No asset class could be loaded from file: %s", file->m_Path.c_str() );
      }
    }
    catch( const Nocturnal::Exception& ex )
    {
      Console::Error( "No asset class could be loaded from file: %s, %s", file->m_Path.c_str(), ex.what() );
    }


    S_tuid shaderIDs;
    AssetManager::GetManagedAssetShaders( assetClass->GetFileID(), shaderIDs );

    for each ( const tuid& shaderID in shaderIDs )
    {
      File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( shaderID );

      if ( file )
      {
        shaderFiles.push_back( file );
      }
    }

    if ( (int) shaderFiles.size() < 1 )
    {
      Console::Error( "No files were found that matched your search criteria.\n" );
      return;
    }
  }
  else
  {
    Console::Error( "No files found with given asset path: %s\n", assetFilePath.c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Finds all assest that use the given shader
//
void ShaderUsages::UsesShader( const std::string& shaderFilePath, File::V_ManagedFilePtr& assetFiles )
{
  tuid shaderID = File::GlobalManager().GetID( shaderFilePath );
  if ( shaderID != TUID::Null )
  {
    S_tuid assetIds;
    g_ShaderUsagesDB->SelectShaderUsages( shaderID, assetIds );

    for each ( const tuid& assetId in assetIds )
    {
      File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( assetId );

      if ( file )
      {
        assetFiles.push_back( file );
      }
    }

    if ( (int) assetFiles.size() < 1 )
    {
      Console::Error( "No files were found that matched your search criteria.\n" );
      return;
    }
  }
  else
  {
    File::V_ManagedFilePtr shaderFiles;
    File::FindManagedFiles( shaderFilePath, shaderFiles );

    if ( !shaderFiles.empty() )
    {
      int shaderIndex = File::DetermineAssetIndex( shaderFiles, "look-up shader usage" );

      S_tuid assetIds;
      g_ShaderUsagesDB->SelectShaderUsages( shaderFiles.at( shaderIndex )->m_Id, assetIds );

      for each ( const tuid& assetId in assetIds )
      {
        File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( assetId );

        if ( file )
        {
          assetFiles.push_back( file );
        }
      }

      if ( (int) assetFiles.size() < 1 )
      {
        Console::Error( "No files were found that matched your search criteria.\n" );
        return;
      }

    }
    else
    {
      Console::Error( "No files found with given shader path: %s\n", shaderFilePath.c_str() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// Load the shader usages DB
//
void ShaderUsages::LoadShaderUsagesDB()
{
  const static char* entityDec = ".entity.irb";

  {
    std::string findSpec = entityDec;
    File::V_ManagedFilePtr assetFiles;
    File::FindManagedFiles( findSpec, assetFiles );

    int count = 0;

    for each ( const File::ManagedFilePtr& file in assetFiles )
    {
      Asset::AssetClassPtr assetClass = NULL;
      try
      {
        if ( ( assetClass = Asset::AssetClass::FindAssetClass( file->m_Id ) ) == NULL )
        {
          continue;
        }
      }
      catch( const Nocturnal::Exception& )
      {
        continue;
      }

      S_tuid shaderIDs;
      AssetManager::GetManagedAssetShaders( assetClass->GetFileID(), shaderIDs );

      for each ( const tuid& shaderID in shaderIDs )
      {
        g_ShaderUsages.insert( M_ShaderUsages::value_type( shaderID, S_tuid() ) );
        g_ShaderUsages[shaderID].insert( file->m_Id );
      }
    }

    g_ShaderUsagesDB->InsertShaderUsages( g_ShaderUsages );
    g_ShaderUsages.clear();
  }
}
