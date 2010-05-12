#include "ShaderRegistry.h"
#include "ShaderUtils.h"

#include "Asset/AnisotropicShaderAsset.h"
#include "Asset/AudioVisualizationShaderAsset.h"
#include "Asset/BRDFShaderAsset.h"
#include "Asset/FoliageShaderAsset.h"
#include "Asset/FurFragmentShaderAsset.h"
#include "Asset/FurShaderAsset.h"
#include "Asset/GroundFogShaderAsset.h"
#include "Asset/GraphShaderAsset.h"
#include "Asset/RefractionShaderAsset.h"
#include "Asset/StandardShaderAsset.h"
#include "Asset/WaterPoolShaderAsset.h"
#include "Asset/OffScreenShaderAsset.h"

// Using
using namespace ShaderProcess;

u32 g_InitCount = 0;

static M_ShaderCreator g_ShaderCreators;
static M_RuntimeTextureCreator g_TextureCreators;

///////////////////////////////////////////////////////////////////////////////
// Helper function to register a shader creation function with a particular
// type of shader.
//
void RegisterShaderCreator( i32 shaderTypeID, MakeShaderBufferFunc creatorFunction )
{
  M_ShaderCreator::iterator inserted = g_ShaderCreators.insert( M_ShaderCreator::value_type( shaderTypeID, creatorFunction ) ).first;
  if ( inserted->second != creatorFunction )
  {
    throw Nocturnal::Exception( "ShaderRegistry - attempted to register the same shader type (%d) to a different shader creation function", shaderTypeID );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to register a texture creation function with a particular
// type of shader and a particular texture slot.
//
void RegisterTextureCreator( i32 shaderTypeID, TextureSlots::TextureSlot slot, MakeRuntimeTextureFunc creatorFunction )
{
  M_RuntimeTextureCreator::iterator inserted = g_TextureCreators.insert( M_RuntimeTextureCreator::value_type( P_TypeToSlot( shaderTypeID, slot ), creatorFunction ) ).first;
  if ( inserted->second != creatorFunction )
  {
    throw Nocturnal::Exception( "ShaderRegistry - attempted to register the same shader type (%d) and slot (%d) to a different texture creation function.", shaderTypeID, static_cast< i32 >( slot ) );
  }
}


void ShaderRegistry::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    // Register all your creator callbacks here

    // StandardShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::StandardShaderAsset >(), &ShaderProcess::MakeStandardShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::StandardShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::StandardShaderAsset >(), TextureSlots::NormalMap, &ShaderProcess::MakeStandardShaderNormalMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::StandardShaderAsset >(), TextureSlots::ExpensiveMap, &ShaderProcess::MakeStandardShaderExpensiveMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::StandardShaderAsset >(), TextureSlots::DetailMap, &ShaderProcess::MakeStandardShaderDetailMapTexture );

    // GroundFogShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::GroundFogShaderAsset >(), &ShaderProcess::MakeGroundFogShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::GroundFogShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );

    // FurShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::FurShaderAsset >(), &ShaderProcess::MakeFurShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::FurShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );

    // RefractionShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::RefractionShaderAsset >(), &ShaderProcess::MakeRefractionShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::RefractionShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );

    // AnisotropicShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::AnisotropicShaderAsset >(), &ShaderProcess::MakeAnisotropicShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::AnisotropicShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::AnisotropicShaderAsset >(), TextureSlots::NormalMap, &ShaderProcess::MakeStandardShaderNormalMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::AnisotropicShaderAsset >(), TextureSlots::AnisotropicExpensiveMap, &ShaderProcess::MakeAnisotropicExpensiveMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::AnisotropicShaderAsset >(), TextureSlots::GrainMap, &ShaderProcess::MakeGrainMapTexture );

    // FurFragmentShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::FurFragmentShaderAsset >(), &ShaderProcess::MakeFurFragmentShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::FurFragmentShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::FurFragmentShaderAsset >(), TextureSlots::NormalMap, &ShaderProcess::MakeStandardShaderNormalMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::FurFragmentShaderAsset >(), TextureSlots::FurControl, &ShaderProcess::MakeFurControlTexture );

    // AudioVisualizationShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::AudioVisualizationShaderAsset >(), &ShaderProcess::MakeAudioVisualShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::AudioVisualizationShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );

    // OffScreenShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::OffScreenShaderAsset >(), &ShaderProcess::MakeOffScreenShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::OffScreenShaderAsset  >(), TextureSlots::ColorMap, &ShaderProcess::MakeOffScreenTexture );

    // FoliageShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::FoliageShaderAsset >(), &ShaderProcess::MakeFoliageShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::FoliageShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::FoliageShaderAsset >(), TextureSlots::NormalMap, &ShaderProcess::MakeStandardShaderNormalMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::FoliageShaderAsset >(), TextureSlots::Foliage, &ShaderProcess::MakeFoliageTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::FoliageShaderAsset >(), TextureSlots::FoliageShadow, &ShaderProcess::MakeFoliageShadowTexture );

    // BRDFShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::BRDFShaderAsset >(), &ShaderProcess::MakeBRDFShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::BRDFShaderAsset >(), TextureSlots::ColorMap, &ShaderProcess::MakeStandardShaderColorMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::BRDFShaderAsset >(), TextureSlots::NormalMap, &ShaderProcess::MakeStandardShaderNormalMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::BRDFShaderAsset >(), TextureSlots::ExpensiveMap, &ShaderProcess::MakeStandardShaderExpensiveMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::BRDFShaderAsset >(), TextureSlots::BRDFMap, &ShaderProcess::MakeBRDFMapTexture );

    // GraphShaderAsset
    RegisterShaderCreator( Reflect::GetType< Asset::GraphShaderAsset >(), &ShaderProcess::MakeGraphShaderBuffer );
    RegisterTextureCreator( Reflect::GetType< Asset::GraphShaderAsset >(), TextureSlots::ColorMap,      &ShaderProcess::MakeStandardShaderColorMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::GraphShaderAsset >(), TextureSlots::NormalMap,     &ShaderProcess::MakeStandardShaderNormalMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::GraphShaderAsset >(), TextureSlots::ExpensiveMap,  &ShaderProcess::MakeStandardShaderExpensiveMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::GraphShaderAsset >(), TextureSlots::DetailMap,     &ShaderProcess::MakeStandardShaderDetailMapTexture );
    RegisterTextureCreator( Reflect::GetType< Asset::GraphShaderAsset >(), TextureSlots::CustomMapA,    &ShaderProcess::MakeCustomMapATexture );
    RegisterTextureCreator( Reflect::GetType< Asset::GraphShaderAsset >(), TextureSlots::CustomMapB,    &ShaderProcess::MakeCustomMapBTexture );
  }
}

void ShaderRegistry::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_ShaderCreators.clear();
    g_TextureCreators.clear();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Looks up the proper function to call and fills out the buffer with data from
// the specified shader.  This class throws an exception if no callback exists
// for a particular shader class.
//
void ShaderRegistry::MakeShaderBuffer( const ShaderCreationParams& args )
{
  M_ShaderCreator::iterator found = g_ShaderCreators.find( args.m_Shader->GetType() );
  if ( found != g_ShaderCreators.end() )
  {
    MakeShaderBufferFunc& func = found->second;
    func( args );
  }
  else
  {
    throw Nocturnal::Exception( "ShaderRegistry does not contain a callback for creating a buffer from shader type %s.", args.m_Shader->GetClass()->m_UIName.c_str() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Looks up the proper function to call and fills out the texture struction (STRUCTION!!!!!)
// with information from the specified shader (for the requested slot).
// Returns false if the default texture should be used.  Unlike MakeShaderBuffer,
// this class does not throw an exception if no callback exists for the requested
// shader and slot; it just returns false.  Returns true if the texture is
// successfully filled out.
//
bool ShaderRegistry::MakeRuntimeTexture( ShaderProcess::RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlots::TextureSlot slot )
{
  bool result = false;
  M_RuntimeTextureCreator::iterator found = g_TextureCreators.find( P_TypeToSlot( shader->GetType(), slot ) );
  if ( found != g_TextureCreators.end() )
  {
    MakeRuntimeTextureFunc& func = found->second;
    result = func( texture, shader, slot );
  }

  return result;
}
