#include "ShaderCache.h"
#include "Exceptions.h" 

#include "AssetClass.h"
#include "StandardShaderAsset.h"

#include "Attribute/AttributeHandle.h"
#include "FileSystem/FileSystem.h"
#include "File/Manager.h"
#include "Finder/Finder.h"
#include "Finder/ShaderSpecs.h"

namespace Asset
{

  REFLECT_DEFINE_CLASS( ShaderCache )

void ShaderCache::EnumerateClass( Reflect::Compositor<ShaderCache>& comp )
{
  Reflect::Field* fieldCacheTimes = comp.AddField( &ShaderCache::m_CacheTimes, "m_CacheTimes" );
  Reflect::Field* fieldShaderNames = comp.AddField( &ShaderCache::m_ShaderNames, "m_ShaderNames" );
}


  ShaderCache::ShaderCache()
  {
  }

  ShaderCachePtr ShaderCache::Load()
  {
    std::string filename = FinderSpecs::Shader::CACHE_FILE.GetFile( Finder::ProjectTemp() );

    ShaderCachePtr shaderCache = FileSystem::Exists( filename ) ? Reflect::Archive::FromFile< ShaderCache >( filename ) : new ShaderCache();

    if ( !shaderCache.ReferencesObject() ) // handle corrupted file by just re-creating it
      shaderCache = new ShaderCache();

    shaderCache->Update();

    return shaderCache;
  }

  void ShaderCache::Update()
  {
    std::string shaderDir = FinderSpecs::Shader::DATA_FOLDER.GetFolder();

    File::V_ManagedFilePtr shaderAssets;
    File::GlobalManager().Find( shaderDir + "*.irb", shaderAssets );

    time_t now;
    time( &now );

    std::set< u64 > usedShaderIds;

    // foreach shader
    for each ( const File::ManagedFilePtr& shaderIt in shaderAssets )
    {
      usedShaderIds.insert( shaderIt->m_Id );

      // if out of date...
      if ( m_CacheTimes.find( shaderIt->m_Id ) == m_CacheTimes.end() || FileSystem::UpdatedSince( shaderIt->m_Path, m_CacheTimes[ shaderIt->m_Id ] ) )
      {
        StandardShaderAssetPtr shader = AssetClass::GetAssetClass<StandardShaderAsset>( shaderIt->m_Id );

        if ( !shader.ReferencesObject() )
        {
          throw Asset::Exception( "Could not load shader: %I64u", shaderIt->m_Id );
        }

        m_ShaderNames[ shaderIt->m_Id ] = shader->GetShortName();
        m_CacheTimes[ shaderIt->m_Id ] = (u32) now;
      }
    }

    // erase any deleted shaders from the cache
    for ( std::map< u64, std::string >::iterator namesIt = m_ShaderNames.begin(); namesIt != m_ShaderNames.end();  )
    {
      // if we find that we have an entry for a now-defunct shader, remove it from the cache times and from the
      // cached shader names
      if ( usedShaderIds.find( (*namesIt).first ) == usedShaderIds.end() )
      {
        std::map< tuid, u32 >::iterator cacheTimesEntry = m_CacheTimes.find( (*namesIt).first );
        if ( cacheTimesEntry != m_CacheTimes.end() )
          m_CacheTimes.erase( cacheTimesEntry );

        namesIt = m_ShaderNames.erase( namesIt );
      }
      else
      {
        ++namesIt;
      }
    }

    // save it
    std::string filename = FinderSpecs::Shader::CACHE_FILE.GetFile( Finder::ProjectTemp() );
    Reflect::Archive::ToFile( this, filename );
  }

  void ShaderCache::GetShaderNames( V_string &shaderNames ) const
  {
    shaderNames.clear();

    for( std::map< u64, std::string >::const_iterator shaderIt = m_ShaderNames.begin(); shaderIt != m_ShaderNames.end(); ++shaderIt )
    {
      shaderNames.push_back( (*shaderIt).second );
    }
  }
}