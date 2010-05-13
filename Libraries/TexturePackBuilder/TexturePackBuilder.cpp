
#define _BOOL bool

#include "TexturePackBuilder.h"

#include "RCS/RCS.h"

#include "Asset/AssetClass.h"
#include "AssetBuilder/Exceptions.h"
#include "Attribute/AttributeHandle.h"
#include "Content/ContentInit.h"

#include "Dependencies/Dependencies.h"
#include "Finder/AssetSpecs.h"
#include "Finder/TexturePackSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Finder/ShaderSpecs.h"
#include "Finder/Finder.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Common/Types.h"
#include "Console/Console.h"
#include "Profile/Profile.h"

using namespace Nocturnal;
using namespace Asset;
using namespace IG;
using namespace Reflect;
using namespace Attribute;

DECLARE_BUILDER_ENTRY_POINTS(TexturePack);

BUILDER_DECL void AllocateBuilders( AssetBuilder::V_IBuilder& builders )
{
  builders.push_back( new AssetBuilder::TexturePack );
}

namespace AssetBuilder
{
  i32 g_InitCount = 0;

  void TexturePack::Initialize()
  {
    if ( ++g_InitCount == 1 )
    {

      Dependencies::Initialize();

      Content::Initialize();
    }
  }

  void TexturePack::Cleanup()
  {
    if ( --g_InitCount == 0 )
    {
      Content::Cleanup();

      Dependencies::Cleanup();

    }
  }

  TexturePack::TexturePack()
  {

  }

  TexturePack::~TexturePack()
  {

  }

  AssetBuilder::BuilderOptionsPtr TexturePack::ParseOptions( const V_string& options )
  {
    // parse command-line options
    m_BuilderOptions = new AssetBuilder::TexturePackBuilderOptions();

    return m_BuilderOptions;
  }

  void TexturePack::Initialize( const Asset::AssetClassPtr& assetClass, BuilderOptionsPtr options )
  {
    CastOptions( options, m_BuilderOptions );

    m_TexturePackAsset = Reflect::ObjectCast<Asset::TexturePackBase>(assetClass);
    if ( !m_TexturePackAsset.ReferencesObject() )
      throw( InvalidAssetClassException( assetClass->m_AssetClassID, "TexturePack" ) );


    std::string builtDir = m_TexturePackAsset->GetBuiltDir();

    if ( m_TexturePackAsset->UseCombinedOutputFile() )
    {
      const Finder::FileSpec& combinedSpec = m_TexturePackAsset->GetCombinedFileSpec();
    
      m_TexelFilename = m_HeaderFilename = combinedSpec.GetFile( builtDir );

      m_OutputFiles.push_back( new Dependencies::FileInfo( m_TexelFilename, combinedSpec ) );
    }
    else
    {
      const Finder::FileSpec& texelSpec = m_TexturePackAsset->GetTexelFileSpec();
      const Finder::FileSpec& headerSpec = m_TexturePackAsset->GetHeaderFileSpec();

      m_TexelFilename    = texelSpec.GetFile( builtDir );
      m_HeaderFilename   = headerSpec.GetFile( builtDir );

      m_OutputFiles.push_back( new Dependencies::FileInfo( m_TexelFilename, texelSpec ) );
      m_OutputFiles.push_back( new Dependencies::FileInfo( m_HeaderFilename, headerSpec ) );
    }

    m_DebugFilename    = FinderSpecs::TexturePack::DEBUG_FILE.GetFile( builtDir );
    m_OutputFiles.push_back( new Dependencies::FileInfo( m_DebugFilename, FinderSpecs::TexturePack::DEBUG_FILE ) );

    V_TextureEntryBase textures;
    m_TexturePackAsset->GetTextures( textures );

    std::map< std::string, V_string > enumToTextures;
    

    for each ( const TextureEntryBase* texture in textures )
    {
      TextureProcess::DefinitionPtr definition = new TextureProcess::Definition;
      texture->ToDefinition( definition );
      
      if ( definition->m_texture_file.empty() )
      {
		    continue;
      }

      m_Bank.m_textures.push_back( definition );

      if ( !definition->m_enum.empty() )
      {
        enumToTextures[ definition->m_enum ].push_back( definition->m_texture_file );
      }
    }
  }

  AssetClass* TexturePack::GetAssetClass()
  {
    return m_TexturePackAsset.Ptr();
  }

  const std::string TexturePack::GetBuildString()
  {
    std::stringstream str;
    str << "Texture Pack '" << m_TexturePackAsset->GetShortName() << "'";
    return str.str();
  }

  void TexturePack::GatherJobs( V_BuildJob& jobs )
  {
    // no dependent builds
  }

  bool TexturePack::IsUpToDate()
  {
    return Dependencies::Graph().AreUpToDate( m_OutputFiles );
  }

  void TexturePack::RegisterInputs( Dependencies::V_DependencyInfo& outputFiles )
  {
    outputFiles.insert( outputFiles.end(), m_OutputFiles.begin(), m_OutputFiles.end() );

    m_LeafInputFiles.push_back( new Dependencies::FileInfo( m_TexturePackAsset->GetFilePath(), FinderSpecs::Asset::CONTENT_FILE, Dependencies::ConfigFlags::LeafInput ) );

    for each ( const TextureProcess::Definition* definition in m_Bank.m_textures )
    {
      m_LeafInputFiles.push_back( new Dependencies::FileInfo( definition->m_texture_file, FinderSpecs::Shader::TEXTURE_FILE, Dependencies::ConfigFlags::LeafInput ) );
    }

    Dependencies::Graph().RegisterInputs( m_OutputFiles, m_LeafInputFiles );
  }

  bool TexturePack::Build()
  {
    BUILDER_SCOPE_TIMER((""));

    ProcessTexturePack();

    return true;
  }

  void TexturePack::ProcessTexturePack()
  {
    if ( !m_Bank.Pack() )
    {
      throw Nocturnal::Exception( "Failed to pack textures!" );
    }

    // why did WriteOutputFiles go away?
    NOC_ASSERT( 0 );
    //m_Bank.WriteOutputFiles( m_HeaderFilename, m_TexelFilename, m_DebugFilename );
  }
}
