#include "Precompile.h"

#include "RemoteAsset.h"
#include "AssetEditor.h"

#include "Task/Build.h"

#include "rpc/interfaces/rpc_lunaview_host.h"
#include "rpc/interfaces/rpc_common.h"

#include "Editor/SessionManager.h"
#include "Editor/Editor.h"
#include "Editor/EditorInfo.h"
#include "ShaderAsset.h"
#include "LevelAsset.h"
#include "SkyAsset.h"
#include "AssetEditor.h"
#include "AssetManager.h"

#include "IPC/TCP.h"
#include "IPC/Pipe.h"
#include "rpc/interfaces/rpc_lunaview_host.h"
#include "File/Manager.h"
#include "AppUtils/AppUtils.h"
#include "Asset/AssetClass.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/SkyShellAttribute.h"

#include "Asset/GrainMapAttribute.h"
#include "Asset/StandardColorMapAttribute.h"
#include "Asset/StandardNormalMapAttribute.h"
#include "Asset/StandardExpensiveMapAttribute.h"
#include "Asset/StandardDetailMapAttribute.h"
#include "Asset/FoliageMapAttribute.h"
#include "Asset/FoliageShadowMapAttribute.h"
#include "Asset/CustomMapsAttributes.h"
#include "Asset/GraphShaderAsset.h"
#include "Asset/EntityAsset.h"

#include "AssetBuilder/ISkyBuilder.h"
#include "BuilderUtil/ColorPalette.h"

#include "Live/RuntimeConnection.h"
#include "Console/Console.h"

#include "igCore/igHeaders/ps3structs.h"
#include "igCore/igHeaders/FileChunkIDs.h" 

#include "ShaderProcess/ShaderProcess.h"
#include "ShaderProcess/ShaderRegistry.h"
#include "ShaderProcess/TextureSlots.h"
#include "TextureProcess/TextureWriter.h"

#include "UIToolKit/DialogWorkerThread.h"
#include "Finder/ShaderSpecs.h" 

using namespace Luna;
using namespace Math;

namespace Luna
{
  S_tuid g_ValidShaders;

  struct LunaShaderTool : RPC::ILunaShaderTool
  {
    LOCAL_IMPL(LunaShaderTool, RPC::ILunaShaderTool);

    virtual void SelectShader(RPC::SelectShaderParam* param);
    virtual void SetValidShaderList(RPC::ValidShaderListParam* param);
    virtual void AddValidShader(RPC::ShaderIdParam* param);
    virtual void RemoveValidShader(RPC::ShaderIdParam* param);
    virtual void ClearValidShaderList();
  };

  void LunaShaderTool::SelectShader( RPC::SelectShaderParam* param )
  {
    if ( RuntimeConnection::IsConnected() )
    {
      g_ValidShaders.insert( param->m_Tuid );

      std::string fileToEdit;

      try
      {
        fileToEdit = File::GlobalManager().GetPath( param->m_Tuid );
      }
      catch ( const File::Exception& e )
      {
        Console::Error( "%s\n", e.what() );
        return;
      }

      SessionManager::GetInstance()->Edit( fileToEdit );
    }
  }

  void LunaShaderTool::SetValidShaderList(RPC::ValidShaderListParam* param)
  {
    g_ValidShaders.clear();

    for ( u32 i = 0;i < param->m_NumShaders; ++i )
    {
      g_ValidShaders.insert( param->m_Shaders[ i ] );
    }
  }

  void LunaShaderTool::AddValidShader(RPC::ShaderIdParam* param)
  {
    g_ValidShaders.insert(param->m_Tuid);
  }

  void LunaShaderTool::RemoveValidShader(RPC::ShaderIdParam* param)
  {
    g_ValidShaders.erase(param->m_Tuid);
  }

  void LunaShaderTool::ClearValidShaderList()
  {
    g_ValidShaders.clear();
  }
}

RemoteAsset::RemoteAsset( AssetEditor* editor )
: RemoteEditor( editor )
, m_AssetEditor( editor )
{
  m_LunaViewHost = RuntimeConnection::GetRemoteLevelView();

  RPC::Host* host = RuntimeConnection::GetHost();

  // create a local implementation of our functionality
  m_LunaShaderHost = RuntimeConnection::GetRemoteShaderView();
  m_LunaShaderTool = new LunaShaderTool( host );
  host->SetLocalInterface( RPC::kLunaShaderTool, m_LunaShaderTool );

  Luna::AssetManager* manager = m_AssetEditor->GetAssetManager();
  manager->AddAssetLoadedListener( AssetLoadSignature::Delegate ( this, &RemoteAsset::AssetLoaded ) );
}

RemoteAsset::~RemoteAsset()
{
  RPC::Host* host = RuntimeConnection::GetHost();
  if ( host )
  {
    host->SetLocalInterface( RPC::kLunaShaderTool, NULL );
  }

  delete m_LunaShaderTool;
}

void RemoteAsset::ShaderChanged( const ShaderChangedArgs& args )
{
  UpdateShader( args.m_ShaderClass, args.m_OldShaderClass, args.m_TextureID );
}

void RemoteAsset::SkyChanged( const SkyChangedArgs& args )
{
  UpdateSky( args.m_SkyClass, args.m_SkyBuilder );
}

void RemoteAsset::SkyLooseGraphShaderChanged( const SkyLooseGraphShaderChangedArgs& args )
{
  UpdateSkyLooseGraphShader( args.m_SkyClass, args.m_SkyBuilder, args.m_LooseGraphShaderClass );
}

void RemoteAsset::SkyTextureChanged( const SkyTextureChangedArgs& args )
{
  UpdateSkyTexture( args.m_SkyClass, args.m_SkyBuilder, (RPC::SkyTextures::SkyTexture)args.m_TextureType, args.m_TexturePath );
}

void RemoteAsset::LevelWeatherAttributesChanged(const LevelWeatherAttributesChangedArgs& args)
{
  SetLevelWeatherAttributes(args.m_LevelClass, args.m_Flags);
}

void RemoteAsset::LevelChanged(const LevelChangedArgs& args)
{
  SetLevelAttributes(args.m_LevelClass);
}

void RemoteAsset::AssetLoaded( const AssetLoadArgs& args )
{
  if ( Luna::ShaderAsset* shaderClass = dynamic_cast< Luna::ShaderAsset* >( args.m_AssetClass ) )
  {
    shaderClass->AddShaderChangedListener( ShaderChangedSignature::Delegate ( this, &RemoteAsset::ShaderChanged ) );
  }
  else if ( Luna::SkyAsset* skyClass = dynamic_cast< Luna::SkyAsset* >( args.m_AssetClass ) )
  {
    skyClass->AddSkyChangedListener( SkyChangedSignature::Delegate ( this, &RemoteAsset::SkyChanged ) );
    skyClass->AddSkyTextureChangedListener( SkyTextureChangedSignature::Delegate ( this, &RemoteAsset::SkyTextureChanged ) );
    skyClass->AddSkyLooseGraphShaderChangedListener( SkyLooseGraphShaderChangedSignature::Delegate ( this, &RemoteAsset::SkyLooseGraphShaderChanged ) );
  }
  else if ( Luna::LevelAsset* levelClass = dynamic_cast< Luna::LevelAsset* >(args.m_AssetClass ) )
  {
    levelClass->AddLevelChangedListener( LevelChangedSignature::Delegate ( this, &RemoteAsset::LevelChanged ) );
    levelClass->AddLevelWeatherAttributesChangedListener( LevelWeatherAttributesChangedSignature::Delegate ( this, &RemoteAsset::LevelWeatherAttributesChanged ) );
  }  
}

///////////////////////////////////////////////////////////////////////////
// Thread for updating a sky texture.
//
class ShaderTextureUpdateThread : public UIToolKit::DialogWorkerThread
{
private:
  Asset::ShaderAssetPtr m_ShaderClass;
  RPC::TextureTypes::TextureType m_TextureType;
  ShaderProcess::RuntimeTexture* m_RuntimeTexture;
  RPC::ILunaShaderHost* m_LunaShaderHost;

public:
  // Constructor
  ShaderTextureUpdateThread( HANDLE evtHandle, wxDialog* dlg, const Asset::ShaderAssetPtr& shaderClass, RPC::TextureType type, ShaderProcess::RuntimeTexture* rt,
                             RPC::ILunaShaderHost* lunaShaderHost )
    : UIToolKit::DialogWorkerThread( evtHandle, dlg )
    , m_ShaderClass( shaderClass )
    , m_TextureType( type )
    , m_RuntimeTexture( rt )
    , m_LunaShaderHost( lunaShaderHost )
  {
  }

  // Acutally does the work of building a shader texture to send to the devkit.
  virtual void DoWork() NOC_OVERRIDE
  {
    IG::Texture* image = ShaderProcess::ShaderGenerator::CreateRuntimeTexture(*m_RuntimeTexture);
    if (image)
    {
      RPC::UpdateShaderTextureParam param;

      param.m_Tuid = m_ShaderClass->m_AssetClassID;

      // generate the mip maps (a full set of)
      IG::MipSet* mips = NULL;
      mips = image->GenerateFinalizedMipSet( m_RuntimeTexture->m_settings, m_RuntimeTexture->m_runtime, m_RuntimeTexture->m_runtime_type == ShaderProcess::TEX_RT_NORMAL_MAP, m_RuntimeTexture->m_runtime_type == ShaderProcess::TEX_RT_D_MAP);
      mips->Swizzle();

      IG::IGTexture  tex;
      u32 texture_crc;

      //Prep the texture for output and get the tex_header, data size and crc
      u32 textureSize;
      TextureProcess::PrepMipSetForOutput(mips, tex, texture_crc, textureSize);

      //copy texture reg settings
      memcpy( &param.m_TextureHeader, &tex, sizeof( tex ) );

      //get texel data
      u8* data = new u8[ textureSize ];
      memset(data,0,textureSize);
      TextureProcess::GetMipSetData(mips, tex, data);

      param.m_Type = m_TextureType;
      m_LunaShaderHost->UpdateShaderTexture( &param, data, textureSize );

      delete [] data;
      delete mips;
      delete image;
    }
  }
};

///////////////////////////////////////////////////////////////////////////
// Thread for updating a sky texture.
//
class SkyTextureUpdateThread : public UIToolKit::DialogWorkerThread
{
private:
  Asset::SkyAssetPtr m_SkyClass;
  AssetBuilder::ISkyBuilder* m_Builder;
  RPC::SkyTextures::SkyTexture m_TextureType;
  std::string m_Path;
  RPC::ILunaViewHost* m_LunaViewHost;

public:
  // Constructor
  SkyTextureUpdateThread( HANDLE evtHandle, wxDialog* dlg, const Asset::SkyAssetPtr& skyClass, AssetBuilder::ISkyBuilder* builder, RPC::SkyTextures::SkyTexture whichTex, const std::string& path,
                          RPC::ILunaViewHost* lunaViewHost )
    : UIToolKit::DialogWorkerThread( evtHandle, dlg )
    , m_SkyClass( skyClass )
    , m_Builder( builder )
    , m_TextureType( whichTex )
    , m_Path( path )
    , m_LunaViewHost( lunaViewHost )
  {
  }

  // Actually does the work of building the sky texture to send to the devkit.
  virtual void DoWork() NOC_OVERRIDE
  {
    if ( RuntimeConnection::IsConnected())
    {
      IG::OutputColorFormat outputFormat = IG::OUTPUT_CF_DXT5;
      bool expandRange = false;
      bool generateMips = true;
      bool convertToLinear = false;

      switch ( m_TextureType )
      {
      case RPC::SkyTextures::LowFreqCloudTexture: // No break
      case RPC::SkyTextures::HighFreqCloudTexture:
        outputFormat = IG::OUTPUT_CF_DXT5;
        expandRange = false;
        generateMips = true;
        convertToLinear = true;
        break;

      case RPC::SkyTextures::TurbulanceTexture:
        outputFormat = IG::OUTPUT_CF_DXT1;
        expandRange = true;
        generateMips = true;
        convertToLinear = false;
        break;

      default:
        // Invalid texture type.  Update this function if a new texture was added to the enum.
        NOC_BREAK();
        break;
      }

      RPC::SkyTextureParam param;
      param.m_SkyTuid        = m_SkyClass->m_AssetClassID;
      param.m_SkyTextureType = m_TextureType;

      if(m_Path.empty() == false)
      {
        IG::MipSet* mips = m_Builder->GenerateMips( m_Path.c_str(), outputFormat, expandRange, generateMips, convertToLinear );
        mips->Swizzle();

        IG::IGTexture tex;
        u32 texture_crc;

        //Prep the texture for output and get the tex_header, data size and crc
        u32 textureSize;
        TextureProcess::PrepMipSetForOutput( mips, tex, texture_crc, textureSize );

        //copy texture reg settings
        memcpy( &param.m_TextureHeader, &tex, sizeof( tex ) );

        //get texel data
        u8* data = new u8[ textureSize ];
        memset( data,0,textureSize );
        TextureProcess::GetMipSetData( mips, tex, data );

        m_LunaViewHost->UpdateSkyTexture( &param, data, textureSize );

        delete [] data;
        delete mips;
      }
      else
      {
        m_LunaViewHost->UpdateSkyTexture( &param, NULL, 0 );
      }
    }
  }
};


///////////////////////////////////////////////////////////////////////////
// Thread for updating a sky loose graph shader.
//
class SkyLooseGraphShaderUpdateThread : public UIToolKit::DialogWorkerThread
{
private:
  Asset::LooseGraphShaderAssetPtr m_LooseGraphShaderClass;
  AssetBuilder::ISkyBuilder*      m_Builder;
  RPC::ILunaViewHost*                  m_LunaViewHost;

public:
  // Constructor
  SkyLooseGraphShaderUpdateThread(  HANDLE                                  evtHandle,
                                    wxDialog*                               dlg, 
                                    const Asset::LooseGraphShaderAssetPtr&  lgs,
                                    AssetBuilder::ISkyBuilder*              builder, 
                                    RPC::ILunaViewHost*                          lunaViewHost )
    : UIToolKit::DialogWorkerThread( evtHandle, dlg )
    , m_Builder( builder )
    , m_LooseGraphShaderClass(lgs)
    , m_LunaViewHost( lunaViewHost )
  {
  }

  // Acutally does the work of building the sky loose graph shader and sending to the devkit.
  virtual void DoWork() NOC_OVERRIDE
  {
    if ( RuntimeConnection::IsConnected()  )
    {
      std::string builtFile;

      if(m_Builder->BuildLooseGraphShader(m_LooseGraphShaderClass, m_LooseGraphShaderClass->m_GraphFile, builtFile))
      {
        RPC::SkyLooseGraphShaderParam param;
        param.m_GraphShaderTuid = m_LooseGraphShaderClass->m_GraphFile;

        IGSerializer built_data(true); 

        //Read the fragment program info 
        built_data.ReadFromFile(builtFile.c_str()); 

        Nocturnal::BasicBufferPtr  src_fp_data =  built_data.GetChunk(IGG::FILECHUNK_FRAGMENT_PROGRAM_DATA,  0);

        if(src_fp_data )
        {
          m_LunaViewHost->UpdateSkyLooseGraphShader( &param, (u8*)src_fp_data->GetData(), src_fp_data->GetSize());
        }
      }
    }
  }
};

///////////////////////////////////////////////////////////////////////////
// Updates the specified texture if it differs from the old shader.
//
void RemoteAsset::UpdateShader( const Asset::ShaderAssetPtr& shaderClass, const Asset::ShaderAssetPtr& oldShaderClass, tuid refreshTextureId )
{
  static bool isUpdating = false;

  if ( !isUpdating && RuntimeConnection::IsConnected() )
  {
    // only update the shader if it's one we know the ps3 has loaded right now
    if ( g_ValidShaders.find( shaderClass->m_AssetClassID ) != g_ValidShaders.end() )
    {
      isUpdating = true;

      ShaderProcess::RuntimeTexture runtimeTextures[MAX_TEXTURE_SLOTS];
      ShaderProcess::TextureParam   textureParams[MAX_TEXTURE_SLOTS];
 
      for(u32 iTex = 0; iTex < MAX_TEXTURE_SLOTS; ++iTex)
      {
        // these functions will only actually send anything if a texture has changed
        if ( UpdateSingleShaderTexture( shaderClass, oldShaderClass, (RPC::TextureType)(RPC::TextureTypes::ColorMap + iTex), runtimeTextures[iTex] ) )
        {
          textureParams[iTex].m_Texture = &runtimeTextures[iTex];
          textureParams[iTex].m_UseTexture = true;
        }
      }

      // updating these are relatively cheap, so always do it
      UpdateShaderParams( shaderClass, textureParams );

      isUpdating = false;
    }
  }
}

///////////////////////////////////////////////////////////////////////////
// Helper function to build a single runtime texture from a shader.
//
bool RemoteAsset::UpdateSingleShaderTexture( const Asset::ShaderAssetPtr& shaderClass, const Asset::ShaderAssetPtr& oldShaderClass, RPC::TextureTypes::TextureType texType, ShaderProcess::RuntimeTexture& rt )
{
  bool isValid = false;

  if ( RuntimeConnection::IsConnected() )
  {
    Asset::TextureMapAttribute* texMapAttr = NULL;
    Asset::TextureMapAttribute* oldTexMapAttr = NULL;
    ShaderProcess::TextureSlots::TextureSlot slot = ShaderProcess::TextureSlots::ColorMap;

    switch ( texType )
    {
    case RPC::TextureTypes::CustomMapA:
      texMapAttr    = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::CustomMapAAttribute >() ) );
      oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::CustomMapAAttribute >() ) );
      slot = ShaderProcess::TextureSlots::CustomMapA;
      break;

    case RPC::TextureTypes::CustomMapB:
      texMapAttr    = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::CustomMapBAttribute >() ) );
      oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::CustomMapBAttribute >() ) );
      slot = ShaderProcess::TextureSlots::CustomMapB;
      break;

    case RPC::TextureTypes::ColorMap:
      texMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );
      oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );
      slot = ShaderProcess::TextureSlots::ColorMap;
      break;

    case RPC::TextureTypes::NormalMap:
      texMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::NormalMapAttribute >() ) );
      oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::NormalMapAttribute >() ) );
      slot = ShaderProcess::TextureSlots::NormalMap;
      break;

    case RPC::TextureTypes::GPIMap:
      texMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::ExpensiveMapAttribute >() ) );
      oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::ExpensiveMapAttribute >() ) );

      if ( !texMapAttr )
      {
        texMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::FoliageMapAttribute >() ) );
      }
      if ( !oldTexMapAttr )
      {
        oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::FoliageMapAttribute >() ) );
      }

      slot = ShaderProcess::TextureSlots::ExpensiveMap;
      break;

    case RPC::TextureTypes::DetailMap:
      texMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::DetailMapAttribute >() ) );
      oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::DetailMapAttribute >() ) );

      // If it's not a detail map, maybe it's a grain map...
      if ( !texMapAttr )
      {
        texMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::GrainMapAttribute >() ) );
      }
      if ( !oldTexMapAttr )
      {
        oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::GrainMapAttribute >() ) );
      }

      // ... or even a foliage shadow map
      if ( !texMapAttr )
      {
        texMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( shaderClass->GetAttribute( Reflect::GetType< Asset::FoliageShadowMapAttribute >() ) );
      }
      if ( !oldTexMapAttr )
      {
        oldTexMapAttr = Reflect::ObjectCast< Asset::TextureMapAttribute >( oldShaderClass->GetAttribute( Reflect::GetType< Asset::FoliageShadowMapAttribute >() ) );
      }

      slot = ShaderProcess::TextureSlots::DetailMap;
      break;
    }

    // If the texture has changed, or the alpha mode on the shader has changed, rebuild the texture.
    if ( texMapAttr && ( texMapAttr->ShouldRebuildTexture( oldTexMapAttr ) || shaderClass->m_AlphaMode != oldShaderClass->m_AlphaMode ) )
    {
      if ( ShaderProcess::ShaderRegistry::MakeRuntimeTexture( rt, shaderClass, slot ) )
      {
        std::string msg( "Sending " );
        msg += texMapAttr->GetFilePath() + " to the viewer...";
        ObjectUpdateDialog dlg( m_AssetEditor, "Updating Shader Texture", msg );

        HANDLE eventHandle = CreateEvent( NULL, TRUE, FALSE, "Shader Update Thread Event" );
        dlg.InitThread( new ShaderTextureUpdateThread( eventHandle, &dlg, shaderClass, texType, &rt, m_LunaShaderHost ) );
        texMapAttr->SetTextureDirty( false );
        dlg.ShowModal();

        isValid = true;
      }
    }
  }

  return isValid;
}

void RemoteAsset::UpdateShaderParams(const Asset::ShaderAssetPtr&  shaderClass, const ShaderProcess::TextureParam* textures)
{
  if ( RuntimeConnection::IsConnected() )
  {
    RPC::UpdateShaderParametersParam param;
    param.m_Tuid = shaderClass->m_AssetClassID;
    Nocturnal::BasicBufferPtr buffer = new Nocturnal::BasicBuffer();

    ShaderProcess::V_TextureParam channels( MAX_TEXTURE_SLOTS );
    channels[ ShaderProcess::TextureSlots::ColorMap     ] = textures[ ShaderProcess::TextureSlots::ColorMap     ];
    channels[ ShaderProcess::TextureSlots::NormalMap    ] = textures[ ShaderProcess::TextureSlots::NormalMap    ];
    channels[ ShaderProcess::TextureSlots::ExpensiveMap ] = textures[ ShaderProcess::TextureSlots::ExpensiveMap ];
    channels[ ShaderProcess::TextureSlots::DetailMap    ] = textures[ ShaderProcess::TextureSlots::DetailMap    ];
    channels[ ShaderProcess::TextureSlots::CustomMapA   ] = textures[ ShaderProcess::TextureSlots::CustomMapA   ];
    channels[ ShaderProcess::TextureSlots::CustomMapB   ] = textures[ ShaderProcess::TextureSlots::CustomMapB   ];

    Asset::ColorMapAttributePtr colorMap = shaderClass->GetAttribute< Asset::ColorMapAttribute >();
    if ( !colorMap.ReferencesObject() || !colorMap->m_Enabled )
    {
      std::string error;
      error = "Color Map attribute is invalid on '" + shaderClass->GetShortName() + "'! Ignoring all shader updates to viewers.";
      wxMessageBox( error.c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, m_AssetEditor );
      return;
    }

    ShaderProcess::ShaderRegistry::MakeShaderBuffer( ShaderProcess::ShaderCreationParams( buffer, shaderClass, channels ) );

    if ( buffer->GetSize() > sizeof( param.m_Shader ) )
    {
      throw Nocturnal::Exception( "Shader buffer size (%d) of '%s' exceeded RPC shader structure size (%d).", buffer->GetSize(), shaderClass->GetFilePath().c_str(), sizeof( param.m_Shader ) );
    }

    // Probably not necessary, just useful if the buffer is not the same size as the destination
    memset( &param.m_Shader, 0, sizeof( param.m_Shader ) );

    memcpy( &param.m_Shader, buffer->GetData(), buffer->GetSize() );

    ShaderAttrTexSettings texSettings;
    memset(&texSettings, 0, sizeof(ShaderAttrTexSettings));

    if ( colorMap.ReferencesObject() )
    {
      UpdateShaderAttrTexSettings( &texSettings, 0, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV, colorMap->m_TexFilter, colorMap->m_MipBias );
    }

    Asset::NormalMapAttributePtr normalMap = shaderClass->GetAttribute< Asset::NormalMapAttribute >();
    if ( !normalMap.ReferencesObject() )
    {
      normalMap = new Asset::StandardNormalMapAttribute();
    }
    UpdateShaderAttrTexSettings( &texSettings, 1, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV, normalMap->m_TexFilter, normalMap->m_MipBias );

    Asset::ExpensiveMapAttributePtr expensiveMap = shaderClass->GetAttribute< Asset::ExpensiveMapAttribute >();
    if ( !expensiveMap.ReferencesObject() )
    {
      expensiveMap = new Asset::StandardExpensiveMapAttribute();
    }
    UpdateShaderAttrTexSettings( &texSettings, 2, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV, expensiveMap->m_TexFilter, expensiveMap->m_MipBias );

    Asset::DetailMapAttributePtr detailMap = shaderClass->GetAttribute< Asset::DetailMapAttribute >();
    if ( !detailMap.ReferencesObject() )
    {
      detailMap = new Asset::StandardDetailMapAttribute();
    }
    UpdateShaderAttrTexSettings( &texSettings, 3, Asset::WrapModes::UV_WRAP, Asset::WrapModes::UV_WRAP, detailMap->m_TexFilter, detailMap->m_MipBias );

    if(MAX_TEXTURE_SLOTS >= 6)
    {
      Asset::CustomMapAAttributePtr customMapA = shaderClass->GetAttribute< Asset::CustomMapAAttribute >();
      if ( !customMapA.ReferencesObject() )
      {
        customMapA = new Asset::CustomMapAAttribute();
      }
      UpdateShaderAttrTexSettings( &texSettings, 4, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV, customMapA->m_TexFilter, customMapA->m_MipBias );

      Asset::CustomMapBAttributePtr CustomMapB = shaderClass->GetAttribute< Asset::CustomMapBAttribute >();
      if ( !CustomMapB.ReferencesObject() )
      {
        CustomMapB = new Asset::CustomMapBAttribute();
      }
      UpdateShaderAttrTexSettings( &texSettings, 5, shaderClass->m_WrapModeU, shaderClass->m_WrapModeV, CustomMapB->m_TexFilter, CustomMapB->m_MipBias );
    }

    memcpy( &param.m_TexSettings, &texSettings, sizeof( param.m_TexSettings ) );

    m_LunaShaderHost->UpdateShaderParameters( &param);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update shader attribute which are part of texture settings on ps3
///////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteAsset::UpdateShaderAttrTexSettings(Luna::ShaderAttrTexSettings* tex_settings,
                                               u32                          tex_index,
                                               Asset::WrapMode              wrap_u,
                                               Asset::WrapMode              wrap_v,
                                               Asset::RunTimeFilter         rt_filter,
                                               float                        mip_bias)
{
  if (wrap_u == Asset::WrapModes::UV_CLAMP)
  {
    tex_settings->m_control1[tex_index].m_wrap_u = IGG::TEXTUREADDRESS_CLAMP;
  }
  else
  {
    tex_settings->m_control1[tex_index].m_wrap_u = IGG::TEXTUREADDRESS_WRAP;
  }

  if (wrap_v == Asset::WrapModes::UV_CLAMP)
  {
    tex_settings->m_control1[tex_index].m_wrap_v = IGG::TEXTUREADDRESS_CLAMP;
  }
  else
  {
    tex_settings->m_control1[tex_index].m_wrap_v = IGG::TEXTUREADDRESS_WRAP;
  }

  switch(rt_filter)
  {
  case Asset::RunTimeFilters::RTF_POINT:
    tex_settings->m_filter[tex_index].m_min_filter = IGG::TEXTUREFILTER_MIN_NEARESTMIPMAPNEAREST;
    tex_settings->m_filter[tex_index].m_mag_filter = IGG::TEXTUREFILTER_MAG_NEAREST;
    break;
  case Asset::RunTimeFilters::RTF_BILINEAR:
    tex_settings->m_filter[tex_index].m_min_filter = IGG::TEXTUREFILTER_MIN_LINEARMIPMAPNEAREST;
    tex_settings->m_filter[tex_index].m_mag_filter = IGG::TEXTUREFILTER_MAG_LINEAR;
    break;
  case Asset::RunTimeFilters::RTF_TRILINEAR:
    tex_settings->m_filter[tex_index].m_min_filter = IGG::TEXTUREFILTER_MIN_LINEARMIPMAPLINEAR;
    tex_settings->m_filter[tex_index].m_mag_filter = IGG::TEXTUREFILTER_MAG_LINEAR;
    break;
  case Asset::RunTimeFilters::RTF_ANISO2_BI:
    tex_settings->m_filter[tex_index].m_min_filter = IGG::TEXTUREFILTER_MIN_LINEARMIPMAPNEAREST;
    tex_settings->m_filter[tex_index].m_mag_filter = IGG::TEXTUREFILTER_MAG_LINEAR;
    tex_settings->m_control1[tex_index].m_anisotropic = 0x02;
    tex_settings->m_control2[tex_index].m_aniso_filter = IGG::ANISOTROPY2;
    break;
  case Asset::RunTimeFilters::RTF_ANISO2_TRI:
    tex_settings->m_filter[tex_index].m_min_filter = IGG::TEXTUREFILTER_MIN_LINEARMIPMAPLINEAR;
    tex_settings->m_filter[tex_index].m_mag_filter = IGG::TEXTUREFILTER_MAG_LINEAR;
    tex_settings->m_control1[tex_index].m_anisotropic = 0x02;
    tex_settings->m_control2[tex_index].m_aniso_filter = IGG::ANISOTROPY2;
    break;
  case Asset::RunTimeFilters::RTF_ANISO4_BI:
    tex_settings->m_filter[tex_index].m_min_filter = IGG::TEXTUREFILTER_MIN_LINEARMIPMAPNEAREST;
    tex_settings->m_filter[tex_index].m_mag_filter = IGG::TEXTUREFILTER_MAG_LINEAR;
    tex_settings->m_control1[tex_index].m_anisotropic = 0x02;
    tex_settings->m_control2[tex_index].m_aniso_filter = IGG::ANISOTROPY4;
    break;
  case Asset::RunTimeFilters::RTF_ANISO4_TRI:
    tex_settings->m_filter[tex_index].m_min_filter = IGG::TEXTUREFILTER_MIN_LINEARMIPMAPLINEAR;
    tex_settings->m_filter[tex_index].m_mag_filter = IGG::TEXTUREFILTER_MAG_LINEAR;
    tex_settings->m_control1[tex_index].m_anisotropic = 0x02;
    tex_settings->m_control2[tex_index].m_aniso_filter = IGG::ANISOTROPY4;
    break;
  }

  i32 mip_bias_fixed = i32(mip_bias * 256.0f);
  mip_bias_fixed = CLAMP(mip_bias_fixed, -4096, 4095);
  tex_settings->m_filter[tex_index].m_mip_bias = mip_bias_fixed;
}

///////////////////////////////////////////////////////////////////////////
// Uses the builder to create an engine structure from the information in
// the specified sky class.  Sends that information to the devkit.
//
void RemoteAsset::UpdateSky( const Asset::SkyAssetPtr& skyClass, AssetBuilder::ISkyBuilder* builder )
{
  if ( skyClass.ReferencesObject() && builder )
  {
    RPC::SkyUpdateParam param;
    memset( &param, 0, sizeof( param ) );
    param.m_SkyTuid = skyClass->GetFileID();

    // Update the sky's settings
    builder->MakeRuntimeDefs( skyClass, param.m_SkyDefs );
    m_LunaViewHost->UpdateSky( &param );

    // Update the settings for each shell
    u32 preShellIndex = 0;
    u32 postShellIndex = 0;
    RPC::SkyShellParam shellParam;
    Attribute::AttributeViewer< Asset::SkyShellAttribute > skyShells( skyClass );
    if ( skyShells.Valid() )
    {
      for ( u32 shellIndex = 0; shellIndex < skyShells->m_SkyShells.size(); ++shellIndex )
      {
        memset( &shellParam, 0, sizeof( shellParam ) );
        Asset::SkyShell* shell = skyShells->m_SkyShells.at( shellIndex );
        shellParam.m_SkyTuid = skyClass->GetFileID();
        if ( shell->m_Type == Asset::SkyShellTypes::PostShell || shell->m_Type == Asset::SkyShellTypes::BloomPostShell )
        {
          shellParam.m_IsPreShell = 0;
          shellParam.m_ShellIndex = postShellIndex++;
        }
        else
        {
          shellParam.m_IsPreShell = 1;
          shellParam.m_ShellIndex = preShellIndex++;
        }

        shellParam.m_Roll = shell->m_Roll;
        shellParam.m_Yaw = shell->m_Yaw;
        shellParam.m_RotationRate = shell->m_RotationRate;

        m_LunaViewHost->UpdateSkyShell( &shellParam );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////
// Displays a modal dialog while updating the graph shader.
//
void RemoteAsset::UpdateSkyLooseGraphShader( const Asset::SkyAssetPtr& skyClass, AssetBuilder::ISkyBuilder* builder, const Asset::LooseGraphShaderAssetPtr& lgs)
{
  std::string msg( "Updating sky graph shader: " );
  msg += File::GlobalManager().GetPath( lgs->m_GraphFile ) + ".";

  ObjectUpdateDialog dlg( m_AssetEditor, "Sky Graph Shader Update", msg );
  dlg.InitThread( new SkyLooseGraphShaderUpdateThread( CreateEvent( NULL,
                                                                    TRUE, 
                                                                    FALSE, 
                                                                    "Sky Graph Shader Update Thread Event" ), 
                                                                    &dlg, 
                                                                    lgs, 
                                                                    builder, 
                                                                    m_LunaViewHost ) );
  dlg.ShowModal();
}

///////////////////////////////////////////////////////////////////////////
// Displays a modal dialog while updating the specified sky texture.
//
void RemoteAsset::UpdateSkyTexture( const Asset::SkyAssetPtr& skyClass, AssetBuilder::ISkyBuilder* builder, RPC::SkyTextures::SkyTexture whichTex, const std::string& path )
{
  std::string msg( "Updating sky texture: " );
  msg += path + ".";

  ObjectUpdateDialog dlg( m_AssetEditor, "Sky Texture Update", msg );
  dlg.InitThread( new SkyTextureUpdateThread( CreateEvent( NULL, TRUE, FALSE, "Sky Texture Update Thread Event" ), &dlg, skyClass, builder, whichTex, path, m_LunaViewHost ) );
  dlg.ShowModal();
}

///////////////////////////////////////////////////////////////////////////
// Updates the foliage settings in real-time.
//
void RemoteAsset::TweakFoliageLod( const Asset::FoliageAsset* foliage )
{
  RPC::FoliageLodParam param;

  param.m_FoliageAssetTuid = foliage->m_AssetClassID;
  param.m_NearLodPlane = foliage->m_NearLodPlane;
  param.m_LodRange = foliage->m_LodRange;
  param.m_LodTransitionRadius = foliage->m_LodTransitionRadius;
  param.m_LodTransitionFactor = foliage->m_LodTransitionFactor;

  m_LunaViewHost->TweakFoliageLod( &param );
}


void RemoteAsset::TriggerAssetView(Asset::AssetClass* assetClass, bool showOptions)
{
  if ( assetClass->GetEngineType() == Asset::EngineTypes::Moby )
  {
    SetMode( RPC::LunaViewModes::MobyView );
  }
  else
  {
    SetMode( RPC::LunaViewModes::Normal );
  }

  Luna::ViewAsset( assetClass->m_AssetClassID, m_AssetEditor, NULL, showOptions );
  // now control will pass to our build/task callbacks:
  // RemoteEditor::BuildFinished
  // RemoteEditor::AssetBuilt
}

Luna::AttachmentPtr RemoteAsset::TriggerMultiMoby(Asset::AssetClass* mainEntityClass, Asset::AssetClass* attachEntityClass)
{
  ClearScene();

  if(!mainEntityClass)
    return NULL;

  Asset::EntityPtr mainEntity = new Asset::Entity;
  mainEntity->SetEntityAssetID( mainEntityClass->GetFileID() );
  mainEntity->m_DefaultName = "main_entity";
  mainEntity->m_ID = UniqueID::TUID::Generate();
  m_Scene.Add( mainEntity );

  Asset::EntityPtr attachEntity;

  if(attachEntityClass)
  {
    attachEntity = new Asset::Entity;
    attachEntity->SetEntityAssetID( attachEntityClass->GetFileID() );
    attachEntity->m_DefaultName = "attach_entity";
    attachEntity->m_ID = UniqueID::TUID::Generate();

    m_Scene.Add( attachEntity );
  }

  m_SelectedEntity = mainEntity;

  // create the permanent record of this attachment in the scene
  //

  Luna::AttachmentPtr attachment = new Luna::Attachment;
  attachment->m_MainEntity = mainEntity;
  attachment->m_AttachedEntity = attachEntity;

  AddAttachment(attachment);

  // update the remote state
  //
  if(m_Enabled)
  {
    m_LunaViewHost->Reset();
  }

  SetMode( RPC::LunaViewModes::MobyView );
  SendScene();

  return attachment;

}


void RemoteAsset::AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args )
{
  __super::AssetBuilt(args);
  m_AssetEditor->GetAssetManager()->AssetBuilt( args );
}