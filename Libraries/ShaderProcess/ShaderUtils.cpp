#include "ShaderUtils.h"
#include "ShaderProcess.h"

#include "Asset/AnisotropicExpensiveMapAttribute.h"
#include "Asset/AnisotropicShaderAsset.h"
#include "Asset/AudioVisualizationShaderAsset.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/CustomMapsAttributes.h"
#include "Asset/BRDFMapAttribute.h"
#include "Asset/BRDFShaderAsset.h"
#include "Asset/FoliageMapAttribute.h"
#include "Asset/FoliageShaderAsset.h"
#include "Asset/FoliageShadowMapAttribute.h"
#include "Asset/FurControlAttribute.h"
#include "Asset/FurFragmentShaderAsset.h"
#include "Asset/FurShaderAsset.h"
#include "Asset/GrainMapAttribute.h"
#include "Asset/GraphShaderAsset.h"
#include "Asset/GroundFogShaderAsset.h"
#include "Asset/RefractionShaderAsset.h"
#include "Asset/StandardColorMapAttribute.h"
#include "Asset/StandardDetailMapAttribute.h"
#include "Asset/StandardExpensiveMapAttribute.h"
#include "Asset/StandardNormalMapAttribute.h"
#include "Asset/StandardShaderAsset.h"
#include "Asset/WaterPoolShaderAsset.h"
#include "Asset/OffScreenShaderAsset.h"
#include "FileSystem/FileSystem.h"
#include "Texture/ColorFormats.h"

#include "igscene/igscene.h"
#include "igCore/igHeaders/ps3structs.h"

using namespace Asset;
using namespace Attribute;

namespace ShaderProcess
{

#define PAD_BUFFER_TO_SHADER_SIZE(buffer_)\
        (buffer_)->PadToArb( sizeof( IGPS3::Shader ) );\
        NOC_ASSERT((buffer_)->GetSize() == sizeof( IGPS3::Shader ))

#define INIT_SHADER_BUFFER(buffer_)\
        (buffer_)->AddU32( 0 ); /* Texture 0 */\
        (buffer_)->AddU32( 0 ); /* Texture 1 */\
        (buffer_)->AddU32( 0 ); /* Texture 2 */\
        (buffer_)->AddU32( 0 ); /* Texture 3 */\
        (buffer_)->AddU32( 0 ); /* Texture 4 */\
        (buffer_)->AddU32( 0 ); /* Texture 5 */\
        (buffer_)->AddU32( 0 ); /* Procedural update function ptr */\
        (buffer_)->AddU32( 0 )  /* Custom render function ptr     */

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Helper function to convert from the array index of a shader's texture to the runtime's
  // equivalent data setting.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  ShaderTextureType TextureSlotToRuntimeTextureType( TextureSlot slot )
  {
    // If you add additional texture slots, you probably need to update
    // this function as well.
    NOC_COMPILE_ASSERT( TextureSlots::NumTextureSlots == 6 );

    ShaderTextureType result = TEX_UNKNOWN;
    switch ( slot )
    {
    case TextureSlots::ColorMap:
      result = TEX_RT_COLOR_MAP;
      break;

    case TextureSlots::NormalMap:
      result = TEX_RT_NORMAL_MAP;
      break;

    case TextureSlots::ExpensiveMap:
      result = TEX_RT_GPI_MAP;
      break;

    case TextureSlots::DetailMap:
      result = TEX_RT_D_MAP;
      break;

    case TextureSlots::CustomMapA:
      result = TEX_RT_CUSTOM_MAP_A;
      break;

    case TextureSlots::CustomMapB:
      result = TEX_RT_CUSTOM_MAP_B;
      break;
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Helper function that returns true if the specified texture is valid to be used in the specified
  // texture channel.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool IsChannelValid( const RuntimeTexture* texture, u32 channel )
  {
    NOC_ASSERT( channel < 4 );

    return texture && texture->m_channel_valid[channel];
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Helper function 
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  u8   EncodeRTSpecularPowerWSMaterial(f32 specular_power, u32 wetsurface_material)
  {
    const f32 c_spec_max  = 81.0f;
    const f32 c_b         = powf(c_spec_max , 1.0f/15.0f);

    specular_power        = CLAMP(specular_power,       1.0f,                               c_spec_max);
    u8  material_index     = CLAMP(wetsurface_material,  WetSurfaceTypes::WET_SURFACE_NONE,  WetSurfaceTypes::WET_SURFACE_METAL);
    u8  specular_index    = (u8)(log(specular_power)/log(c_b));
    

    return ((specular_index << 3) | material_index);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the buffer with an IGPS3::Shader struct.
  // Texture indices must be written back into the buffer by the caller.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeStandardShaderBuffer( const ShaderCreationParams& args )
  {
    using namespace TextureSlots;

    // You must pass in information for all texture channels
    NOC_ASSERT( args.m_TextureSlots.size() == NumTextureSlots );

    AttributeViewer< StandardColorMapAttribute >      colorMap( args.m_Shader );
    AttributeViewer< StandardNormalMapAttribute>      normalMap( args.m_Shader, true );
    AttributeViewer< StandardExpensiveMapAttribute >  expensiveMap( args.m_Shader, true );
    AttributeViewer< StandardDetailMapAttribute >     detailMap( args.m_Shader, true );

    // Enabled channels
    bool enableNormal =
      args.m_Shader->ContainsAttribute( Reflect::GetType< StandardNormalMapAttribute >() ) &&
      (normalMap->m_NormalMapScale > 0.0f) && !normalMap->GetFilePath().empty();

    bool enableGloss =
      args.m_Shader->ContainsAttribute( Reflect::GetType< StandardExpensiveMapAttribute >() ) &&
      expensiveMap->m_GlossMapEnabled && (expensiveMap->m_GlossScale > 0.0f);

    bool enableParallax =
      expensiveMap->m_ParallaxMapEnabled &&
      args.m_Shader->ContainsAttribute( Reflect::GetType< StandardExpensiveMapAttribute >() );

    bool enableIncan =
      args.m_Shader->ContainsAttribute( Reflect::GetType< StandardExpensiveMapAttribute >() ) &&
      expensiveMap->m_IncanMapEnabled;

    bool enableDetail =
      args.m_Shader->ContainsAttribute( Reflect::GetType< StandardDetailMapAttribute >() ) &&
      !detailMap->GetFilePath().empty();

    // Disable values based upon whether we have texture information or not.
    // The reason for this complex logic is that when we are doing a real build
    // of the shader, we will have already generated runtime textures which we
    // should use to set some of the shader values.  However, in the case of doing
    // real time updates within Luna, we may not have built the textures (since
    // it's slow to do so), so we cannot change the enable state based upon the
    // texture information since we do not have the texture information.
    if ( args.m_TextureSlots[NormalMap].m_UseTexture )
    {
      enableNormal &= args.m_TextureSlots[NormalMap].m_Texture != NULL;
    }
    if ( args.m_TextureSlots[ExpensiveMap].m_UseTexture )
    {
      enableGloss &= IsChannelValid( args.m_TextureSlots[ExpensiveMap].m_Texture, 0 );
      enableParallax &= IsChannelValid( args.m_TextureSlots[ExpensiveMap].m_Texture, 1 );
      enableIncan &= IsChannelValid( args.m_TextureSlots[ExpensiveMap].m_Texture, 2 );
    }
    if ( args.m_TextureSlots[DetailMap].m_UseTexture )
    {
      enableDetail &= args.m_TextureSlots[DetailMap].m_Texture != NULL;
    }

    // Illegal to have parallax without normal map
    enableParallax &= enableNormal;

    // Flags
    u8 flags = 0;
    if ( args.m_Shader->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }
    if ( !enableIncan )
    {
      flags |= IGPS3::SHF_NO_INCANDESENCE;
    }
    if ( colorMap->m_DisableBaseTint )
    {
      flags |= IGPS3::SHF_DISABLE_ENV_TINT;
    }

    // Shader Type
    u8 shaderType = 0;
    if ( enableParallax )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_P;
    }
    if ( enableGloss )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_G_ACTIVE;
    }
    if ( enableNormal )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_N;
    }
    if ( enableDetail )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_F;
    }

    // This is done so that the parallax map results in a MAD instead of two instructions
    // N = (vertex_height - bias) * height_percent
    // X = -(height_percent * bias)
    // N = vertex_height * height_percent + X // a MADD instruction
    float parallaxBias = -( expensiveMap->m_ParallaxScale * expensiveMap->m_ParallaxBias );

    //
    // Serialize to IGPS3::Shader struct (see ps3structs.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( shaderType );                  // m_shader_type
    buffer->AddU8( args.m_Shader->m_AlphaMode );  // m_alpha_type
    buffer->AddU8( 0 );                           // m_custom_type
    buffer->AddU8( flags );                       // m_flags

    // derive real-time spec power from cube map spec type if it's set to an invalid value (which is the default)
    float spec_power = (expensiveMap->m_RealTimeSpecPower > 1.0f) ? expensiveMap->m_RealTimeSpecPower : powf(3.0f, float(expensiveMap->m_CubeSpecType + 1.0f));
    spec_power = Math::Clamp(spec_power, 1.f, 127.f);

    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.r / 255.0f) );     // m_base_map_tint
    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.g / 255.0f) );     //
    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.b / 255.0f) );     //

    buffer->AddF32( normalMap->m_NormalMapScale );                          // m_normal_map_scale
    buffer->AddF32( expensiveMap->m_GlossScale );                           // m_gloss_scale

    u32 material_type     = args.m_Shader->m_WetSurfaceMode;
    u8  specular_material = EncodeRTSpecularPowerWSMaterial(spec_power, material_type);
    buffer->AddU8( specular_material );                                     // m_specular_material
    buffer->AddU8( 0                 );                                     // pads
    buffer->AddU8( 0                 );                                     // pads
    buffer->AddU8( 0                 );                                     // pads

    buffer->AddF32( detailMap->m_DetailStrength );                          // m_fine_detail_strength
    buffer->AddU32( (u32)expensiveMap->m_CubeSpecType );                    // m_cube_spec_type
    buffer->AddU32( 0 );                                                    // unused
    buffer->AddU32( 0 );                                                    // unused
    buffer->AddU32( 0 );                                                    // unused

    float glossDirty = Math::Clamp( expensiveMap->m_GlossDirty, 0.0f, 1.0f );
    buffer->AddF32( SrgbToLinear(expensiveMap->m_GlossTint.r / 255.0f) );   // m_gloss_tint_dirty
    buffer->AddF32( SrgbToLinear(expensiveMap->m_GlossTint.g / 255.0f) );   //
    buffer->AddF32( SrgbToLinear(expensiveMap->m_GlossTint.b / 255.0f) );   //
    buffer->AddF32( glossDirty );                                           //

    buffer->AddF32( expensiveMap->m_RimStrength );
    buffer->AddF32( expensiveMap->m_RimTightness );
    buffer->AddF32( expensiveMap->m_CubeBias );                             // m_cube_bias
    buffer->AddF32( expensiveMap->m_RealTimeSpecIntensity * 4.0f );         // m_specular_scale - factor of 4.0 is to balance the fact that cubemaps are encoded at 4x luminance

    buffer->AddF32( expensiveMap->m_ParallaxScale );                        // m_parallax_scale
    buffer->AddF32( parallaxBias );                                         // m_parallax_bias
    buffer->AddF32( detailMap->m_ResolutionScale );                         // m_fine_detail_scale
    buffer->AddU32( 0 );                                                    // unused

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the buffer with a CustomShaderGroundFog struct.
  // Texture indices must be written back into the buffer by the caller.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeGroundFogShaderBuffer( const ShaderCreationParams& args )
  {
    Asset::GroundFogShaderAsset* groundFog = Reflect::AssertCast< Asset::GroundFogShaderAsset >( args.m_Shader );

    // ground fog shader only has one texture (a color map)
    AttributeViewer< StandardColorMapAttribute > colorMap( groundFog );

    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( groundFog->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }

    float scroll_rot = groundFog->m_UvScrollDir * (PI / 180.0f);
    CLAMP(scroll_rot, 0.0f, PI * 2.0f);
    float scroll_du = sinf(scroll_rot) * groundFog->m_UvScrollSpeed;
    float scroll_dv = -cosf(scroll_rot) * groundFog->m_UvScrollSpeed;

    //
    // Serialize the CustomShaderGroundFog struct in ps3 format (see igg/igCustomShaders.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( 0 );                                             // m_allowed
    buffer->AddU8( groundFog->m_AlphaMode );                        // m_alpha_type
    buffer->AddU8( ShaderProcess::CustomShaderTypes::GroundFog );   // m_custom_type
    buffer->AddU8( flags );                                         // m_flags
    buffer->AddU32( 0 );                                            // procedural func id (converted to function pointer at runtime)

    buffer->AddU32( groundFog->m_UseUvs );                          // m_use_uvs
    buffer->AddF32( groundFog->m_UvScale );                         // m_uv_scale
    buffer->AddF32( scroll_du );                                    // m_uv_scroll_du
    buffer->AddF32( scroll_dv );                                    // m_uv_scroll_dv
    buffer->AddF32( groundFog->m_Bumpiness );                       // m_bumpiness
    buffer->AddF32( groundFog->m_BumpZero );                        // m_bump_zero
    buffer->AddF32( groundFog->m_Gradient );                        // m_gradient
    buffer->AddF32( groundFog->m_MidRatio );                        // m_mid_ratio
    buffer->AddF32( colorMap->m_BaseMapTint.r );                    // m_fog_color
    buffer->AddF32( colorMap->m_BaseMapTint.g );                    //
    buffer->AddF32( colorMap->m_BaseMapTint.b );                    //
    buffer->AddF32( 1.0f );                                         //

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the buffer with a CustomShaderFur struct.
  // Texture indices must be written back into the buffer by the caller.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeFurShaderBuffer( const ShaderCreationParams& args )
  {
    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( args.m_Shader->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }

    //
    // Serialize the CustomShaderFur struct in ps3 format (see igg/igCustomShaders.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( 0 );                                       // m_allowed
    buffer->AddU8( args.m_Shader->m_AlphaMode );              // m_alpha_type
    buffer->AddU8( ShaderProcess::CustomShaderTypes::Fur );   // m_custom_type
    buffer->AddU8( flags );                                   // m_flags

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the buffer with a CustomShaderRefraction struct.
  // Texture indices must be written back into the buffer by the caller.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeRefractionShaderBuffer( const ShaderCreationParams& args )
  {
    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( args.m_Shader->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }

    //
    // Serialize the CustomShaderRefraction struct in ps3 format (see igg/igCustomShaders.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( 0 );                                             // m_allowed
    buffer->AddU8( args.m_Shader->m_AlphaMode );                    // m_alpha_type
    buffer->AddU8( ShaderProcess::CustomShaderTypes::Refraction );  // m_custom_type
    buffer->AddU8( flags );                                         // m_flags

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills the buffer with a fur fragment shader (new fur).
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeFurFragmentShaderBuffer( const ShaderCreationParams& args )
  {
    Asset::FurFragmentShaderAsset* furFrag = Reflect::AssertCast< Asset::FurFragmentShaderAsset >( args.m_Shader );
    AttributeViewer< StandardColorMapAttribute > colorMap( furFrag );

    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( furFrag->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }
    if ( colorMap->m_DisableBaseTint )
    {
      flags |= IGPS3::SHF_DISABLE_ENV_TINT;
    }

    //
    // Serialize the CustomShaderFur2 struct in ps3 format (see igg/igCustomShaders.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8 ( 0 );                                                    // m_allowed
    buffer->AddU8 ( furFrag->m_AlphaMode );                                 // m_alpha_type
    buffer->AddU8 ( ShaderProcess::CustomShaderTypes::Fur2 );               // m_custom_type
    buffer->AddU8 ( flags );                                                // m_flags

    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.r / 255.0f) );     // m_base_tint
    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.g / 255.0f) );
    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.b / 255.0f) );
    buffer->AddF32( 1.0f );

    buffer->AddF32( furFrag->m_FurLength );                                 // m_fur_length
    buffer->AddU32( furFrag->m_Layers );                                    // m_layers
    buffer->AddF32( furFrag->m_FurDensity );                                // m_fur_density
    buffer->AddF32( furFrag->m_OffsetMapBiasU );                            // m_offsetmap_biasu
    buffer->AddF32( furFrag->m_OffsetMapBiasV );                            // m_offsetmap_biasv
    buffer->AddF32( furFrag->m_LightFactor1 );                              // m_light_factor1
    buffer->AddF32( furFrag->m_LightFactor2 );                              // m_light_factor2

    buffer->AddF32( SrgbToLinear(furFrag->m_SpecTint.r / 255.0f) );         // m_spec_tint
    buffer->AddF32( SrgbToLinear(furFrag->m_SpecTint.g / 255.0f) );
    buffer->AddF32( SrgbToLinear(furFrag->m_SpecTint.b / 255.0f) );
    buffer->AddF32( 1.0f );

    buffer->AddF32( furFrag->m_FurTangentScale );                           // m_fur_tangent_scale

    buffer->AddF32( furFrag->m_LodStartDist );                              // m_fur_lod_start_dist
    buffer->AddF32( furFrag->m_LodEndDist );                                // m_fur_lod_end_dist
    buffer->AddF32( furFrag->m_LodLengthScale );                            // m_fur_lod_length_scale
    buffer->AddU32( furFrag->m_LodLayerCount );                             // m_fur_lod_layer_count

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills the buffer with a CustomShaderAnisotropic structure.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeAnisotropicShaderBuffer( const ShaderCreationParams& args )
  {
    Asset::AnisotropicShaderAsset* anisotropic = Reflect::AssertCast< Asset::AnisotropicShaderAsset >( args.m_Shader );
    AttributeViewer< StandardColorMapAttribute > colorMap( anisotropic );
    AttributeViewer< AnisotropicExpensiveMapAttribute > expensiveMap( anisotropic );

    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( anisotropic->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }
    if ( colorMap->m_DisableBaseTint )
    {
      flags |= IGPS3::SHF_DISABLE_ENV_TINT;
    }

    //
    // Serialize the CustomShaderAnisotropic struct in ps3 format (see igg/igCustomShaders.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8 ( 0 );                                                    // m_allowed
    buffer->AddU8 ( anisotropic->m_AlphaMode );                             // m_alpha_type
    buffer->AddU8 ( ShaderProcess::CustomShaderTypes::Anisotropic );        // m_custom_type
    buffer->AddU8 ( flags );                                                // m_flags

    buffer->AddF32( anisotropic->m_BackFaceAdjust );                        // m_back_face_adjust

    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.r / 255.0f) );     // m_base_tint
    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.g / 255.0f) );
    buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.b / 255.0f) );

    buffer->AddF32( expensiveMap->m_SpecularPower );                        // m_spec_power

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills the buffer with a CustomShaderAudioVisual structure.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeAudioVisualShaderBuffer( const ShaderCreationParams& args )
  {
    Asset::AudioVisualizationShaderAsset* audioVisual = Reflect::AssertCast< Asset::AudioVisualizationShaderAsset >( args.m_Shader );

    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( audioVisual->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }

    // clear flag - start with a clean texture or accumulate old one
    const u8 clear = audioVisual->m_Clear ? 1 : 0;

    //
    // Serialize the CustomShaderAudioVisual struct in ps3 format (see igg/igCustomShaders.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( 0 );                                             // m_allowed
    buffer->AddU8( audioVisual->m_AlphaMode );                      // m_alpha_type
    buffer->AddU8 ( ShaderProcess::CustomShaderTypes::AudioVisual );// m_custom_type
    buffer->AddU8 ( flags );                                        // m_flags

    // Custom data
    buffer->AddU32( 0 );                                            // m_surface_data

    buffer->AddU8( 0 );                                             // m_read_index
    buffer->AddU8( clear );                                         // m_clear
    buffer->AddU8( 0 );                                             // m_frag_prog
    buffer->AddU8( 0 );                                             // m_bank
    buffer->AddU16( audioVisual->m_Width );                         // m_width
    buffer->AddU16( audioVisual->m_Height );                        // m_height

    buffer->AddU8( audioVisual->m_AccumulateFactor.r );             //  m_accumulate_factor
    buffer->AddU8( audioVisual->m_AccumulateFactor.g );
    buffer->AddU8( audioVisual->m_AccumulateFactor.b );
    buffer->AddU8( (u8)255 );

    buffer->AddU8( audioVisual->m_FeedbackColor.r );                //  m_feedback_color
    buffer->AddU8( audioVisual->m_FeedbackColor.g );
    buffer->AddU8( audioVisual->m_FeedbackColor.b );
    buffer->AddU8( (u8)255 );

    buffer->AddF32( audioVisual->m_FeedbackX );                     // m_feedback_pos
    buffer->AddF32( audioVisual->m_FeedbackY );

    buffer->AddF32( audioVisual->m_FeedbackU );                     // m_feedback_uv
    buffer->AddF32( audioVisual->m_FeedbackV );

    buffer->AddU8( audioVisual->m_Waveform1->m_Color.r );           // m_waveform1_color
    buffer->AddU8( audioVisual->m_Waveform1->m_Color.g );
    buffer->AddU8( audioVisual->m_Waveform1->m_Color.b );
    buffer->AddU8( (u8)255 );

    buffer->AddU8( audioVisual->m_Waveform2->m_Color.r );           // m_waveform2_color
    buffer->AddU8( audioVisual->m_Waveform2->m_Color.g );
    buffer->AddU8( audioVisual->m_Waveform2->m_Color.b );
    buffer->AddU8( (u8)255 );

    buffer->AddF32( audioVisual->m_Waveform1->m_PosX );             // m_waveform_posx_ch1
    buffer->AddF32( audioVisual->m_Waveform1->m_PosY );             // m_waveform_posy_ch1
    buffer->AddF32( audioVisual->m_Waveform1->m_Scale );            // m_waveform_scale_ch1
    buffer->AddF32( audioVisual->m_Waveform1->m_Rotate );           // m_waveform_rotate_ch1

    buffer->AddF32( audioVisual->m_Waveform2->m_PosX );             // m_waveform_posx_ch2
    buffer->AddF32( audioVisual->m_Waveform2->m_PosY );             // m_waveform_posy_ch2
    buffer->AddF32( audioVisual->m_Waveform2->m_Scale );            // m_waveform_scale_ch2
    buffer->AddF32( audioVisual->m_Waveform2->m_Rotate );           // m_waveform_rotate_ch2

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the buffer with a CustomShaderOffScreen struct.
  // Texture indices must be written back into the buffer by the caller.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeOffScreenShaderBuffer( const ShaderCreationParams& args )
  {
    Asset::OffScreenShaderAsset* offScreen = Reflect::AssertCast< Asset::OffScreenShaderAsset >( args.m_Shader );

    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( args.m_Shader->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }

    // Default width and height values.  We will load the texture and fetch the
    // real width and height if available.
    u32 width = 256;
    u32 height = 256;

    if ( args.m_TextureSlots.size() > 0 && args.m_TextureSlots.front().m_UseTexture )
    {
      AttributeViewer< StandardColorMapAttribute > colorMap( offScreen );
      if ( colorMap.Valid() && !colorMap->GetFilePath().empty() )
      {
        IG::Texture* colorMapTex = IG::Texture::LoadFile( colorMap->GetFilePath().c_str(), false, NULL ); // texture is not actually used, don't bother with gamma correction
        if ( colorMapTex )
        {
          width = colorMapTex->m_Width;
          height = colorMapTex->m_Height;
          delete colorMapTex;
        }
      }
    }

    //
    // Serialize the CustomShaderOffScreen struct in ps3 format (see igg/igCustomShaders.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( 0 );                                             // m_allowed
    buffer->AddU8( args.m_Shader->m_AlphaMode );                    // m_alpha_type
    buffer->AddU8( ShaderProcess::CustomShaderTypes::OffScreen );   // m_custom_type
    buffer->AddU8( flags );                                         // m_flags

    // Custom data
    buffer->AddU32( width );                                        // m_width
    buffer->AddU32( height );                                       // m_height
    buffer->AddU32( offScreen->m_BinkID );                          // m_bink_id

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////

  static inline u16 FloatToU16(float v, float range)
  {
    // return a u16 to represent a float from 0 to range
    v /= range;
    v *= 65535.f;
    v = CLAMP(v, 0.f, 65535.f);
    return u16(v);
  }

  void MakeFoliageShaderBuffer( const ShaderCreationParams& args )
  {
    Asset::FoliageShaderAsset* foliage = Reflect::AssertCast< Asset::FoliageShaderAsset >( args.m_Shader );
    AttributeViewer< FoliageShadowMapAttribute > shadow( foliage );

    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( args.m_Shader->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }

    foliage->m_BackDiffuseSpread = CLAMP(foliage->m_BackDiffuseSpread, 0.f, 1.f);
    foliage->m_WindStrength = CLAMP(foliage->m_WindStrength, 0.f, 2.f);
    shadow->m_ShadowSpreadMin = CLAMP(shadow->m_ShadowSpreadMin, -16.f, 16.f);
    shadow->m_ShadowSpreadMax = CLAMP(shadow->m_ShadowSpreadMax, 0.f, 32.f);

    //
    // Serialize the FoliageShader struct in ps3 format (see igg/igFoliage.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( IGPS3::IG_ASSET_TYPE_FOLIAGE );                  // foliage only
    buffer->AddU8( 0 );                                             // always opaque
    buffer->AddU8( ShaderProcess::CustomShaderTypes::Foliage );     // m_custom_type
    buffer->AddU8( flags );                                         // m_flags

    buffer->AddU8((u8)foliage->m_AxisType );                       // m_axis_type
    buffer->AddU8( u8(foliage->m_WindStrength * (255.f / 2.f)) );   // m_wind_strength
    buffer->AddI8( i8(shadow->m_ShadowSpreadMin) );                 // m_shadow_spread_min
    buffer->AddI8( i8(shadow->m_ShadowSpreadMax) );                 // m_shadow_spread_max

    buffer->AddU16( FloatToU16(foliage->m_AmbientSpread, 1.f) );    // m_ambient_spread
    buffer->AddU16( 0 );
    buffer->AddU16( FloatToU16(foliage->m_BackDiffuseSpread, 1.f) );// m_back_diffuse_spread
    buffer->AddU16( foliage->m_ShadowActive ? 1 : 0);               // m_shadow_active
    buffer->AddF32( shadow->m_ShadowSpreadUVScale );                // m_shadow_uv_scale
    buffer->AddF32( shadow->m_ShadowSpreadUVOffset );               // m_shadow_uv_offset

    float r, g, b;
    foliage->m_AmbientColor.ToLinearFloat(r, g, b);
    buffer->AddF32( r );                                            // m_ambient_color
    buffer->AddF32( g );                                            // m_ambient_color
    buffer->AddF32( b );                                            // m_ambient_color
    buffer->AddF32( 1.f );                                          // m_ambient_color

    foliage->m_DiffuseColor.ToLinearFloat(r, g, b);
    buffer->AddF32( r );                                            // m_diffuse_color
    buffer->AddF32( g );                                            // m_diffuse_color
    buffer->AddF32( b );                                            // m_diffuse_color
    buffer->AddF32( 1.f );                                          // m_diffuse_color

    foliage->m_BackDiffuseColor.ToLinearFloat(r, g, b);
    buffer->AddF32( r );                                            // m_back_diffuse_color
    buffer->AddF32( g );                                            // m_back_diffuse_color
    buffer->AddF32( b );                                            // m_back_diffuse_color
    buffer->AddF32( 1.f );                                          // m_back_diffuse_color

    foliage->m_SpecularColor.ToLinearFloat(r, g, b);
    buffer->AddF32( r );                                            // m_specular_color
    buffer->AddF32( g );                                            // m_specular_color
    buffer->AddF32( b );                                            // m_specular_color
    buffer->AddF32( 1.f );                                          // m_specular_color

    buffer->AddF32( foliage->m_SpecularPower );                     // m_specular_power
    buffer->AddF32( foliage->m_ShadowStrength );

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeBRDFShaderBuffer( const ShaderCreationParams& args )
  {
    BRDFShaderAsset* shader = Reflect::AssertCast< BRDFShaderAsset >( args.m_Shader );
    AttributeViewer< StandardColorMapAttribute > colorMap( shader );
    AttributeViewer< BRDFMapAttribute > brdfMap( shader );
    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    // compose flags word in ps3 format
    u32 flags = IGPS3::SHF_CUSTOM_SHADER;
    if ( args.m_Shader->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }
    if ( colorMap->m_DisableBaseTint )
    {
      flags |= IGPS3::SHF_DISABLE_ENV_TINT;
    }

    if ( colorMap.Valid() && brdfMap.Valid() )
    {
      float uOffset = brdfMap->m_UOffset;
      float vOffset = brdfMap->m_VOffset;
      float specDirty = brdfMap->m_SpecularDirty;

      // Shader header
      INIT_SHADER_BUFFER(buffer);

      buffer->AddU8( 0 );                           // m_allowed
      buffer->AddU8( args.m_Shader->m_AlphaMode );  // m_alpha_type
      buffer->AddU8( CustomShaderTypes::BRDF );     // m_custom_type
      buffer->AddU8( flags );                       // m_flags

      // Custom data
      buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.r / 255.0f) );  // m_base_tint
      buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.g / 255.0f) );
      buffer->AddF32( SrgbToLinear(colorMap->m_BaseMapTint.b / 255.0f) );
      buffer->AddF32( 1.0 );

      buffer->AddF32( SrgbToLinear(shader->m_TransmitColor.r / 255.0f) );  // m_transmit_col
      buffer->AddF32( SrgbToLinear(shader->m_TransmitColor.g / 255.0f) );
      buffer->AddF32( SrgbToLinear(shader->m_TransmitColor.b / 255.0f) );
      buffer->AddF32( 1.0 );

      buffer->AddF32( Math::Clamp( uOffset, -1.0, 1.0 ) );  // m_u_offset
      buffer->AddF32( Math::Clamp( vOffset, -1.0, 1.0 ) );  // m_v_offset
      buffer->AddF32( Math::Clamp( specDirty, 0.0, 1.0 ) ); // m_spec_dirty
      buffer->AddF32( brdfMap->m_SpecularPower );           // m_spec_power

      buffer->AddF32( SrgbToLinear(brdfMap->m_SpecularTint.r / 255.0f) ); // m_spec_tint
      buffer->AddF32( SrgbToLinear(brdfMap->m_SpecularTint.g / 255.0f) );
      buffer->AddF32( SrgbToLinear(brdfMap->m_SpecularTint.b / 255.0f) );
      buffer->AddF32( 1.0 );
    }

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  void MakeGraphShaderBuffer( const ShaderCreationParams& args )
  {
    GraphShaderAsset*                                 shader = Reflect::AssertCast< GraphShaderAsset >( args.m_Shader );
    AttributeViewer< StandardColorMapAttribute >      baseAttr( args.m_Shader );
    AttributeViewer< StandardNormalMapAttribute>      normalAttr( args.m_Shader);
    AttributeViewer< StandardExpensiveMapAttribute >  expensiveAttr( args.m_Shader);
    AttributeViewer< StandardDetailMapAttribute >     detailAttr( args.m_Shader);

    // Enabled channels
    bool enableNormal   = normalAttr.Valid()    && (normalAttr->m_NormalMapScale > 0.0f) && !normalAttr->GetFilePath().empty();
    bool enableGloss    = expensiveAttr.Valid() &&  expensiveAttr->m_GlossMapEnabled && (expensiveAttr->m_GlossScale > 0.0f);
    bool enableParallax = expensiveAttr.Valid() &&  expensiveAttr->m_ParallaxMapEnabled;
    bool enableIncan    = expensiveAttr.Valid() &&  expensiveAttr->m_IncanMapEnabled;
    bool enableDetail   = detailAttr.Valid()    && !detailAttr->GetFilePath().empty();

    if ( args.m_TextureSlots[TextureSlots::NormalMap].m_UseTexture )
    {
      enableNormal &= args.m_TextureSlots[TextureSlots::NormalMap].m_Texture != NULL;
    }
    if ( args.m_TextureSlots[TextureSlots::ExpensiveMap].m_UseTexture )
    {
      enableGloss     &= IsChannelValid( args.m_TextureSlots[TextureSlots::ExpensiveMap].m_Texture, 0 );
      enableParallax  &= IsChannelValid( args.m_TextureSlots[TextureSlots::ExpensiveMap].m_Texture, 1 );
      enableIncan     &= IsChannelValid( args.m_TextureSlots[TextureSlots::ExpensiveMap].m_Texture, 2 );
    }
    if ( args.m_TextureSlots[TextureSlots::DetailMap].m_UseTexture )
    {
      enableDetail &= args.m_TextureSlots[TextureSlots::DetailMap].m_Texture != NULL;
    }

    // Illegal to have parallax without normal map
    enableParallax &= enableNormal;

    // Flags
    u8 flags  = IGPS3::SHF_CUSTOM_SHADER;

    if ( args.m_Shader->m_DoubleSided )
    {
      flags |= IGPS3::SHF_TWO_SIDED;
    }

    if ( !enableIncan )
    {
      flags |= IGPS3::SHF_NO_INCANDESENCE;
    }

    if ( baseAttr.Valid() && baseAttr->m_DisableBaseTint )
    {
      flags |= IGPS3::SHF_DISABLE_ENV_TINT;
    }

    // Shader Type
    u8 shaderType = 0;
    if ( enableParallax )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_P;
    }
    if ( enableGloss )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_G_ACTIVE;
    }
    if ( enableNormal )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_N;
    }
    if ( enableDetail )
    {
      shaderType |= IGPS3::IG_SHADER_TYPE_MASK_F;
    }

    // This is done so that the parallax map results in a MAD instead of two instructions
    // N = (vertex_height - bias) * height_percent
    // X = -(height_percent * bias)
    // N = vertex_height * height_percent + X // a MADD instruction
    float parallaxBias = 0.0f;

    if (enableParallax)
    {
      parallaxBias = -( expensiveAttr->m_ParallaxScale * expensiveAttr->m_ParallaxBias );
    }

    //
    // Serialize to IGPS3::Shader struct (see ps3structs.h)
    //

    const Nocturnal::BasicBufferPtr& buffer = args.m_Buffer;

    INIT_SHADER_BUFFER(buffer);

    buffer->AddU8( shaderType );                                    // m_shader_type
    buffer->AddU8( args.m_Shader->m_AlphaMode );                    // m_alpha_type
    buffer->AddU8( CustomShaderTypes::Graph );                      // m_custom_type
    buffer->AddU8( flags );                                         // m_flags

    // Custom data
    float resolutionScale       = 1.0f;
    float detailStrength        = 0.0f;
    if(enableDetail)
    {
      resolutionScale = detailAttr->m_ResolutionScale;
      detailStrength  = detailAttr->m_DetailStrength;
    }

    float specPower     = 1.0f;
    float specScale     = 1.0f;
    float parallaxScale = 1.0f;
    float glossScale    = 1.0f;
    float glossDirty    = 0.0f;
    float glossTintR    = 0.0f;
    float glossTintG    = 0.0f;
    float glossTintB    = 0.0f;
    float cubeBias      = 0.0f;
    u32   cubeSpecType  = 0;

    if(expensiveAttr.Valid())
    {
      // derive real-time spec power from cube map spec type if it's set to an invalid value (which is the default)
      float specPower = (expensiveAttr->m_RealTimeSpecPower > 1.0f) ? expensiveAttr->m_RealTimeSpecPower : powf(3.0f, float(expensiveAttr->m_CubeSpecType + 1.0f));
      specPower = Math::Clamp(specPower, 1.f, 127.f);
      specScale     = expensiveAttr->m_RealTimeSpecIntensity;
      parallaxScale = expensiveAttr->m_ParallaxScale;
      glossScale    = expensiveAttr->m_GlossScale;
      glossDirty    = Math::Clamp( expensiveAttr->m_GlossDirty, 0.0f, 1.0f );
      glossTintR    = expensiveAttr->m_GlossTint.r;
      glossTintG    = expensiveAttr->m_GlossTint.g;
      glossTintB    = expensiveAttr->m_GlossTint.b;
      cubeBias      = expensiveAttr->m_CubeBias;
      cubeSpecType  = (u32)expensiveAttr->m_CubeSpecType;
    }
    
    tuid  graphShaderFileID   = shader->m_GraphFile;
    u32   captureLiveBuffers  = shader->GetCaptureFlags();

    if(baseAttr.Valid())
    {
      buffer->AddF32( SrgbToLinear(baseAttr->m_BaseMapTint.r / 255.0f) );     // m_base_map_tint
      buffer->AddF32( SrgbToLinear(baseAttr->m_BaseMapTint.g / 255.0f) );     //
      buffer->AddF32( SrgbToLinear(baseAttr->m_BaseMapTint.b / 255.0f) );     //
    }
    else
    {
      buffer->AddF32( 0.0f );     // m_base_map_tint
      buffer->AddF32( 0.0f );     //
      buffer->AddF32( 0.0f );     //
    }

    if(normalAttr.Valid())
    {
      buffer->AddF32( normalAttr->m_NormalMapScale );                        // m_normal_map_scale
    }
    else
    {
      buffer->AddF32( 1.0f );                                               // m_normal_map_scale
    }

    buffer->AddF32( glossScale );                                           // m_gloss_scale
    u32 material_type     = args.m_Shader->m_WetSurfaceMode;
    u8  specular_material = EncodeRTSpecularPowerWSMaterial(specPower, material_type);
    buffer->AddU8( specular_material );                                     // m_specular_material
    buffer->AddU8( 0                 );                                     // pads
    buffer->AddU8( 0                 );                                     // pads
    buffer->AddU8( 0                 );                                     // pads

    buffer->AddF32( detailStrength );                                       // m_fine_detail_strength

    buffer->AddU32( cubeSpecType );                                         // m_cube_spec_type
    buffer->AddU32( captureLiveBuffers );
    buffer->AddU64( graphShaderFileID );

    buffer->AddF32( SrgbToLinear(glossTintR / 255.0f) );                    // m_gloss_tint_dirty
    buffer->AddF32( SrgbToLinear(glossTintG / 255.0f) );                    //
    buffer->AddF32( SrgbToLinear(glossTintB / 255.0f) );                    //
    buffer->AddF32( glossDirty );                                           //

    if(expensiveAttr.Valid())
    {
      buffer->AddF32( expensiveAttr->m_RimStrength );
      buffer->AddF32( expensiveAttr->m_RimTightness );
      buffer->AddF32( expensiveAttr->m_CubeBias     );                       // m_cube_bias
    }
    else
    {
      buffer->AddF32( 0.0f); 
      buffer->AddF32( 0.0f); 
      buffer->AddF32( 0.0f);    
    }

    buffer->AddF32( specScale * 4.0f );                                     // m_specular_scale

    // Custom data
    buffer->AddF32( parallaxScale );                                        // m_parallax_scale
    buffer->AddF32( parallaxBias );                                         // m_parallax_bias
    buffer->AddF32( resolutionScale );                                      // m_fine_detail_scale
    buffer->AddU32( 0 );                                                    // graph shader custom inputs - set at runtime

    PAD_BUFFER_TO_SHADER_SIZE(buffer);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void SetTexGenReductionRatio(Asset::ReductionRatio reduction, IG::TextureGenerationSettings& settings)
  {
    // extract texture scaling options
    switch (reduction)
    {
    case ReductionRatios::ONE_ONE:
      settings.m_scale = 1.0f;
      break;

    case ReductionRatios::ONE_HALF:
      settings.m_scale = 0.5f;
      break;

    case ReductionRatios::ONE_FOURTH:
      settings.m_scale = 0.25f;
      break;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // - Sets mip settings for defs for output partition.
  // - mip settings can be applied to all four possible output partitions seperately
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void SetTexGenMipSettings(Asset::MipGenFilterType         mip_filter,
                            Asset::PostMipFilterType        image_filter,
                            V_i32                           mip_filter_passes,
                            IG::TextureGenerationSettings&  settings,
                            u32                             partition)
  {
    NOC_ASSERT(partition < 4);
    // set default
    settings.m_ifilter_cnt[partition][0] = 0;
    for (u32 i=1;i<IG::MAX_TEXTURE_MIPS;i++)
      settings.m_ifilter_cnt[partition][i] = 1;

    settings.m_mip_filter[partition] = (IG::FilterType)mip_filter;
    settings.m_image_filter[partition] = (IG::PostMipImageFilter)image_filter;
    for (u32 i=0;i<mip_filter_passes.size();i++)
      settings.m_ifilter_cnt[partition][i] = mip_filter_passes[i];

    if (settings.m_mip_filter[partition] == IG::MIP_FILTER_NONE)
    {
      settings.m_generate_mips = false;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void SetTexRuntimeSettings(Asset::RunTimeFilter             filter,
                             WrapMode                         wrap_u,
                             WrapMode                         wrap_v,
                             float                            mip_bias,
                             IG::MipSet::RuntimeSettings&     rs)
  {
    rs.m_direct_uvs = false;
    rs.m_expand_range = false;

    rs.m_wrap_w = IG::UV_WRAP;

    // convert wrap modes
    if ( wrap_u == WrapModes::UV_CLAMP )
    {
      rs.m_wrap_u = IG::UV_CLAMP;
    }
    else
    {
      rs.m_wrap_u = IG::UV_WRAP;
    }

    if ( wrap_v == WrapModes::UV_CLAMP )
      rs.m_wrap_v = IG::UV_CLAMP;
    else
      rs.m_wrap_v = IG::UV_WRAP;

    switch (filter)
    {
    case RunTimeFilters::RTF_POINT:
      rs.m_filter = IG::FILTER_POINT_SELECT_MIP;
      break;
    case RunTimeFilters::RTF_BILINEAR:
      rs.m_filter = IG::FILTER_LINEAR_SELECT_MIP;
      break;
    case RunTimeFilters::RTF_TRILINEAR:
      rs.m_filter = IG::FILTER_LINEAR_LINEAR_MIP;
      break;
    case RunTimeFilters::RTF_ANISO2_BI:
      rs.m_filter = IG::FILTER_ANISO_2_SELECT_MIP;
      break;
    case RunTimeFilters::RTF_ANISO2_TRI:
      rs.m_filter = IG::FILTER_ANISO_2_LINEAR_MIP;
      break;
    case RunTimeFilters::RTF_ANISO4_BI:
      rs.m_filter = IG::FILTER_ANISO_4_SELECT_MIP;
      break;
    case RunTimeFilters::RTF_ANISO4_TRI:
      rs.m_filter = IG::FILTER_ANISO_4_LINEAR_MIP;
      break;
    }

    rs.m_mip_bias = mip_bias;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the texture runtime/generation settings with default values.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  void SetTextureSettingsToDefaults( IG::MipSet::RuntimeSettings& texRunSettings, IG::TextureGenerationSettings& texGenSettings )
  {
    SetTexRuntimeSettings( RunTimeFilters::RTF_BILINEAR, WrapModes::UV_WRAP, WrapModes::UV_WRAP, 0.0f, texRunSettings );
    SetTexGenReductionRatio( ReductionRatios::ONE_ONE, texGenSettings );

    texGenSettings.m_output_format = IG::OUTPUT_CF_ARGB8888;
    texGenSettings.m_scale = 1.0f;
    texGenSettings.m_max_size = 1024;
    V_i32 passes;
    for (u32 ichannel = 0; ichannel < 4; ++ichannel)
    {
      SetTexGenMipSettings( MipGenFilterTypes::MIP_POINT, PostMipFilterTypes::POST_NOCHANGE, passes, texGenSettings, ichannel );
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the RuntimeTexture for the color map channel of a StandardShaderAsset.
  // Returns true if the structure was filled out, false if the default texture should be used.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeStandardShaderColorMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< StandardColorMapAttribute > colorMap( shader );

    if ( !colorMap.Valid() )
    {
      return false;
    }

    ShaderProcess::SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    ShaderProcess::SetTexRuntimeSettings( colorMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, colorMap->m_MipBias, texture.m_runtime );

    ShaderProcess::SetTexGenReductionRatio( colorMap->m_ReductionRatio, texture.m_settings );
    ShaderProcess::SetTexGenMipSettings( colorMap->m_MipGenFilter, colorMap->m_PostMipFilter, colorMap->m_MipFilterPasses, texture.m_settings, 0 );
    ShaderProcess::SetTexGenMipSettings( colorMap->m_MipGenFilter, colorMap->m_PostMipFilter, colorMap->m_MipFilterPasses, texture.m_settings, 1 );
    ShaderProcess::SetTexGenMipSettings( colorMap->m_MipGenFilter, colorMap->m_PostMipFilter, colorMap->m_MipFilterPasses, texture.m_settings, 2 );
    ShaderProcess::SetTexGenMipSettings( colorMap->m_AlphaMipGenFilter, colorMap->m_AlphaPostMipFilter, colorMap->m_AlphaMipFilterPasses, texture.m_settings, 3 );
    texture.m_settings.m_output_format = ( IG::OutputColorFormat )( colorMap->m_TexFormat );

    // Max limit on size.
    if ( shader->IsCinematicShader() )
    {
      texture.m_settings.m_max_size = 2048;
    }
    else
    {
      texture.m_settings.m_max_size = 1024;
    }

    AttributeViewer< StandardNormalMapAttribute > normalMap( shader, true );

    texture.m_processing_type = TEX_RT_COLOR_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    texture.m_aux_data[0] = colorMap->m_AmbOccScale;
    texture.m_aux_data[1] = 0.0f;
    texture.m_aux_data[2] = 0.0f;
    texture.m_aux_data[3] = 0.0f;
    texture.m_aux_flag = false;

    // set srgb->linear conversion at runtime
    texture.m_runtime.m_srgb_expand_a = false;
    texture.m_runtime.m_srgb_expand_r = true;
    texture.m_runtime.m_srgb_expand_g = true;
    texture.m_runtime.m_srgb_expand_b = true;

    bool color,alpha;
    std::string colorMapFilePath = colorMap->GetFilePath();
    if ( !colorMapFilePath.empty() )
    {
      // there is a color map specified, use it to fill the RGB channels
      texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_BLUE]=TEXTURE_CHANNEL_BLUE;

      texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_GREEN]=TEXTURE_CHANNEL_GREEN;

      texture.m_channel_textures[TEXTURE_CHANNEL_RED] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_RED]=TEXTURE_CHANNEL_RED;
      color = true;

      texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_ALPHA]=TEXTURE_CHANNEL_ALPHA;
      alpha = true;
    }
    else
    {
      // there is no color map, use the default
      texture.m_channel_textures[TEXTURE_CHANNEL_BLUE]="";
      texture.m_channels[TEXTURE_CHANNEL_BLUE]=0xffffffff;
      texture.m_channel_textures[TEXTURE_CHANNEL_GREEN]="";
      texture.m_channels[TEXTURE_CHANNEL_GREEN]=0xffffffff;
      texture.m_channel_textures[TEXTURE_CHANNEL_RED]="";
      texture.m_channels[TEXTURE_CHANNEL_RED]=0xffffffff;
      color = false;
      texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]="";
      texture.m_channels[TEXTURE_CHANNEL_ALPHA]=0xffffffff;
      alpha = false;
    }

    //if normal map exists then set the aux textures for color maps
    std::string normalMapFilePath = normalMap->GetFilePath();
    if ( !normalMapFilePath.empty() )
    {
      texture.m_aux_channel_texture = normalMapFilePath;
    }
    else
    {
      texture.m_aux_channel_texture="";
    }

    if ( !shader->HasType( Reflect::GetType< WaterPoolShaderAsset >() ) )
    {
      //Check if this is a graphshader
      GraphShaderAsset* graphShader = Reflect::ObjectCast< GraphShaderAsset >( shader );

      //Don't force Alpha to one if we are dealing with a graphshader
      if (!alpha || 
          ((shader->m_AlphaMode == AlphaTypes::ALPHA_OPAQUE) &&
           (graphShader == NULL)))
      {
        texture.m_runtime.m_alpha_channel = IG::COLOR_CHANNEL_FORCE_ONE;
      }
    }

    if (color==false && alpha == false)
    {
      // we can use the default texture for this RT texture
      return false;
    }

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the RuntimeTexture for the normal map channel of a StandardShaderAsset.
  // Returns true if the structure was filled out, false if the default texture should be used.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeStandardShaderNormalMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< StandardNormalMapAttribute > normalMap( shader );
    if ( !normalMap.Valid() || normalMap->GetFilePath().empty() )
    {
      return false;
    }

    ShaderProcess::SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );
    ShaderProcess::SetTexRuntimeSettings( normalMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, normalMap->m_MipBias, texture.m_runtime );

    // expand to the -1 to +1 range, to save some instructions in the pixel shader
    // PC ignores this
    texture.m_runtime.m_expand_range = true;

    // map alpha channel to r component at runtime for normal maps
    texture.m_runtime.m_swizzle_override.m_reg_alpha = 0;
    texture.m_runtime.m_swizzle_override.m_reg_red = 0;
    texture.m_runtime.m_swizzle_override.m_reg_green = 2;
    texture.m_runtime.m_swizzle_override.m_reg_blue = 3;

    // special case for A8L8 - map blue channel to runtime green
    if ( normalMap->m_TexFormat == NormalTexFormats::NM_AL88 )
    {
      texture.m_runtime.m_swizzle_override.m_reg_green = 3;
    }

    texture.m_runtime.m_swizzle_override.m_alpha_enable = 1;
    texture.m_runtime.m_swizzle_override.m_red_enable = 1;
    texture.m_runtime.m_swizzle_override.m_green_enable = 1;

    ShaderProcess::SetTexGenReductionRatio( normalMap->m_ReductionRatio, texture.m_settings );
    ShaderProcess::SetTexGenMipSettings( normalMap->m_MipGenFilter, normalMap->m_PostMipFilter, normalMap->m_MipFilterPasses, texture.m_settings, 0 );
    ShaderProcess::SetTexGenMipSettings( normalMap->m_MipGenFilter, normalMap->m_PostMipFilter, normalMap->m_MipFilterPasses, texture.m_settings, 1 );
    ShaderProcess::SetTexGenMipSettings( normalMap->m_MipGenFilter, normalMap->m_PostMipFilter, normalMap->m_MipFilterPasses, texture.m_settings, 2 );
    ShaderProcess::SetTexGenMipSettings( normalMap->m_MipGenFilter, normalMap->m_PostMipFilter, normalMap->m_MipFilterPasses, texture.m_settings, 3 );

    texture.m_settings.m_output_format  = ( IG::OutputColorFormat )( normalMap->m_TexFormat );

    // Max limit on size for standard shaders.
    if ( shader->IsCinematicShader() )
    {
      texture.m_settings.m_max_size = 2048;
    }
    else
    {
      texture.m_settings.m_max_size = 1024;
    }

    // the runtime normal map is made from the art normal map directly
    // the texture settings come from the shader as is.
    texture.m_processing_type = TEX_RT_NORMAL_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    std::string normalMapFilePath = normalMap->GetFilePath();
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = normalMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_BLUE]=TEXTURE_CHANNEL_BLUE;
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = normalMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_GREEN]=TEXTURE_CHANNEL_GREEN;
    texture.m_channel_textures[TEXTURE_CHANNEL_RED] = normalMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_RED]=TEXTURE_CHANNEL_RED;
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]="";
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]=0xffffffff;

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the RuntimeTexture for the expensive map channel of a StandardShaderAsset.
  // Returns true if the structure was filled out, false if the default texture should be used.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeStandardShaderExpensiveMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< StandardExpensiveMapAttribute > expensiveMap( shader );

    if ( !expensiveMap.Valid() )
    {
      return false;
    }

    if ( expensiveMap->GetFilePath().empty() )
    {
      return false;
    }

    if ( !expensiveMap->m_ParallaxMapEnabled
      && !expensiveMap->m_GlossMapEnabled
      && !expensiveMap->m_IncanMapEnabled
      && !expensiveMap->m_DetailMapMaskEnabled )
    {
      return false;
    }

    ShaderProcess::SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    ShaderProcess::SetTexRuntimeSettings( expensiveMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, expensiveMap->m_MipBias, texture.m_runtime );

    // Gloss, Para and Incan

    IG::OutputColorFormat gpiFormat = ( IG::OutputColorFormat )expensiveMap->m_TexFormat;
    if ( !expensiveMap->m_DetailMapMaskEnabled )
    {
      // if detail map mask is not enabled, force the detail mask data in the alpha channel to one
      texture.m_runtime.m_alpha_channel = IG::COLOR_CHANNEL_FORCE_ONE;
    }
    else if ( gpiFormat == IG::OUTPUT_CF_DXT1 )
    {
      // if detal map mask is enabled, switch from dxt1 to dxt5 so mask data can be encoded in the alpha channel
      gpiFormat = IG::OUTPUT_CF_DXT5;
    }


    // Texure generation settings
    ShaderProcess::SetTexGenReductionRatio( expensiveMap->m_ReductionRatio, texture.m_settings );
    ShaderProcess::SetTexGenMipSettings( expensiveMap->m_MipGenFilter, expensiveMap->m_PostMipFilter, expensiveMap->m_MipFilterPasses, texture.m_settings, 0 );
    ShaderProcess::SetTexGenMipSettings( expensiveMap->m_IncanMipGenFilter, expensiveMap->m_IncanPostMipFilter, expensiveMap->m_IncanMipFilterPasses, texture.m_settings, 1 );
    ShaderProcess::SetTexGenMipSettings( expensiveMap->m_ParaMipGenFilter, expensiveMap->m_ParaPostMipFilter, expensiveMap->m_ParaMipFilterPasses, texture.m_settings, 2 );
    ShaderProcess::SetTexGenMipSettings( expensiveMap->m_DetailMaskMipGenFilter, expensiveMap->m_DetailMaskPostMipFilter, expensiveMap->m_DetailMaskMipFilterPasses, texture.m_settings, 3 );

    texture.m_settings.m_output_format = gpiFormat;

    // Max limit on size.
    if ( shader->IsCinematicShader() )
    {
      texture.m_settings.m_max_size = 2048;
    }
    else
    {
      texture.m_settings.m_max_size = 1024;
    }

    // gloss channel goes throug srgb->linear conversion at runtime -- after some discussion it was decided that this gives the most intuitive response for the artists
    //  note, the texture will still be processed in the tools in srgb space -- only textures with r, g and b set to expand will be processed in linear space
    texture.m_runtime.m_srgb_expand_a = false;
    texture.m_runtime.m_srgb_expand_r = true;
    texture.m_runtime.m_srgb_expand_g = false;
    texture.m_runtime.m_srgb_expand_b = false;

    texture.m_processing_type = TEX_RT_GPI_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    // the RGB of extra goes into RBG of GPI map
    // Incan In Blue (ART has it in GREEN)
    std::string expensiveMapFilePath = expensiveMap->GetFilePath();
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_BLUE]=TEXTURE_CHANNEL_GREEN;
    // Parallax in Green (ART has it in BLUE)
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_GREEN]=TEXTURE_CHANNEL_BLUE;
    texture.m_channel_textures[TEXTURE_CHANNEL_RED] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_RED]=TEXTURE_CHANNEL_RED;
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]=TEXTURE_CHANNEL_ALPHA;
    if ( !expensiveMap->m_IncanMapEnabled )
    {
      texture.m_runtime.m_blue_channel = IG::COLOR_CHANNEL_FORCE_ZERO;
    }

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the RuntimeTexture for the detail map channel of a StandardShaderAsset.
  // Returns true if the structure was filled out, false if the default texture should be used.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeStandardShaderDetailMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< StandardDetailMapAttribute > detailMap( shader );
    if ( !detailMap.Valid() || detailMap->GetFilePath().empty() )
    {
      return false;
    }

    ShaderProcess::SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    ShaderProcess::SetTexRuntimeSettings( detailMap->m_TexFilter, WrapModes::UV_WRAP, WrapModes::UV_WRAP, detailMap->m_MipBias, texture.m_runtime );

    // expand to the -1 to +1 range, to save some instructions in the pixel shader
    // PC ignores this
    texture.m_runtime.m_expand_range = true;

    // backwards compatibility...
    DetailTexFormat detailFormat = detailMap->m_TexFormat;
    const Reflect::Enumeration* detailTextFormatEnum = Reflect::GetEnumeration< DetailTexFormats::DetailTexFormat >();
    if ( detailTextFormatEnum->m_ElementsByValue.find( detailFormat ) == detailTextFormatEnum->m_ElementsByValue.end() )
    {
      detailFormat = DetailTexFormats::D_ARGB8888;
    }

    ShaderProcess::SetTexGenReductionRatio( detailMap->m_ReductionRatio, texture.m_settings );
    ShaderProcess::SetTexGenMipSettings( detailMap->m_MipGenFilter, detailMap->m_PostMipFilter, detailMap->m_MipFilterPasses, texture.m_settings, 0 );
    ShaderProcess::SetTexGenMipSettings( detailMap->m_MipGenFilter, detailMap->m_PostMipFilter, detailMap->m_MipFilterPasses, texture.m_settings, 1 );
    ShaderProcess::SetTexGenMipSettings( detailMap->m_MipGenFilter, detailMap->m_PostMipFilter, detailMap->m_MipFilterPasses, texture.m_settings, 2 );
    ShaderProcess::SetTexGenMipSettings( detailMap->m_MipGenFilter, detailMap->m_PostMipFilter, detailMap->m_MipFilterPasses, texture.m_settings, 3 );
    texture.m_settings.m_output_format = ( IG::OutputColorFormat )( detailFormat );

    // Max limit on size.
    if ( shader->IsCinematicShader() )
    {
      texture.m_settings.m_max_size = 2048;
    }
    else
    {
      texture.m_settings.m_max_size = 1024;
    }

    texture.m_processing_type = TEX_RT_D_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    std::string detailMapFilePath = detailMap->GetFilePath();
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = detailMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_BLUE] = TEXTURE_CHANNEL_BLUE;
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = detailMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_GREEN] = TEXTURE_CHANNEL_GREEN;
    texture.m_channel_textures[TEXTURE_CHANNEL_RED] = detailMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_RED] = TEXTURE_CHANNEL_RED;
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA] = detailMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]=TEXTURE_CHANNEL_ALPHA;

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the runtime texture structure for a shader that has an anisotropic expensive map.
  // Returns true if the structure could be filled out; returns false if there was not an anisotropic
  // expensive map on the specified shader.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeAnisotropicExpensiveMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< AnisotropicExpensiveMapAttribute > expensiveMap( shader );

    if ( !expensiveMap.Valid() || expensiveMap->GetFilePath().empty() )
    {
      return false;
    }

    SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    SetTexRuntimeSettings( expensiveMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, expensiveMap->m_MipBias, texture.m_runtime );

    SetTexGenReductionRatio( expensiveMap->m_ReductionRatio, texture.m_settings );
    SetTexGenMipSettings( expensiveMap->m_MipGenFilter, expensiveMap->m_PostMipFilter, expensiveMap->m_MipFilterPasses, texture.m_settings, 0 );
    SetTexGenMipSettings( expensiveMap->m_MipGenFilter, expensiveMap->m_PostMipFilter, expensiveMap->m_MipFilterPasses, texture.m_settings, 1 );
    SetTexGenMipSettings( expensiveMap->m_MipGenFilter, expensiveMap->m_PostMipFilter, expensiveMap->m_MipFilterPasses, texture.m_settings, 2 );
    SetTexGenMipSettings( expensiveMap->m_MipGenFilter, expensiveMap->m_PostMipFilter, expensiveMap->m_MipFilterPasses, texture.m_settings, 3 );
    texture.m_settings.m_output_format = ( IG::OutputColorFormat )( expensiveMap->m_TexFormat );

    // gloss channel goes throug srgb->linear conversion at runtime -- after some discussion it was decided that this gives the most intuitive response for the artists
    //  note, the texture will still be processed in the tools in srgb space -- only textures with r, g and b set to expand will be processed in linear space
    texture.m_runtime.m_srgb_expand_a = false;
    texture.m_runtime.m_srgb_expand_r = true;
    texture.m_runtime.m_srgb_expand_g = false;
    texture.m_runtime.m_srgb_expand_b = false;

    texture.m_processing_type = TEX_RT_GPI_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    // the RGB of extra goes into RBG of GPI map
    // Incan In Blue (ART has it in GREEN)
    std::string expensiveMapFilePath = expensiveMap->GetFilePath();
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_BLUE]=TEXTURE_CHANNEL_GREEN;
    // Parallax in Green (ART has it in BLUE)
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_GREEN]=TEXTURE_CHANNEL_BLUE;
    texture.m_channel_textures[TEXTURE_CHANNEL_RED] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_RED]=TEXTURE_CHANNEL_RED;
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA] = expensiveMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]=TEXTURE_CHANNEL_ALPHA;

    if ( !expensiveMap->m_IncanMapEnabled )
    {
      texture.m_runtime.m_blue_channel = IG::COLOR_CHANNEL_FORCE_ZERO;
    }

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the runtime texture structure for a shader that has a grain map.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeGrainMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< GrainMapAttribute > grainMap( shader );

    if ( !grainMap.Valid() || grainMap->GetFilePath().empty() )
    {
      return false;
    }

    SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    SetTexRuntimeSettings( grainMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, grainMap->m_MipBias, texture.m_runtime );

    SetTexGenReductionRatio( grainMap->m_ReductionRatio, texture.m_settings );
    SetTexGenMipSettings( grainMap->m_MipGenFilter, grainMap->m_PostMipFilter, grainMap->m_MipFilterPasses, texture.m_settings, 0 );
    SetTexGenMipSettings( grainMap->m_MipGenFilter, grainMap->m_PostMipFilter, grainMap->m_MipFilterPasses, texture.m_settings, 1 );
    SetTexGenMipSettings( grainMap->m_MipGenFilter, grainMap->m_PostMipFilter, grainMap->m_MipFilterPasses, texture.m_settings, 2 );
    SetTexGenMipSettings( grainMap->m_MipGenFilter, grainMap->m_PostMipFilter, grainMap->m_MipFilterPasses, texture.m_settings, 3 );
    texture.m_settings.m_output_format = ( IG::OutputColorFormat )( grainMap->m_TexFormat );

    texture.m_processing_type = TEX_RT_COLOR_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    std::string grainMapFilePath = grainMap->GetFilePath();
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = grainMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_BLUE] = TEXTURE_CHANNEL_BLUE;
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = grainMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_GREEN] = TEXTURE_CHANNEL_GREEN;
    texture.m_channel_textures[TEXTURE_CHANNEL_RED] = grainMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_RED] = TEXTURE_CHANNEL_RED;
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA] = grainMapFilePath;
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]=TEXTURE_CHANNEL_ALPHA;

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Fills out the runtime texture structure for a fur control texture map.  This is used on
  // the fur fragment shader.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeFurControlTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< FurControlAttribute > furControl( shader );

    if ( !furControl.Valid() )
    {
      return false;
    }

    SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    SetTexRuntimeSettings( furControl->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, furControl->m_MipBias, texture.m_runtime );

    SetTexGenReductionRatio( furControl->m_ReductionRatio, texture.m_settings );
    SetTexGenMipSettings( furControl->m_MipGenFilter, furControl->m_PostMipFilter, furControl->m_MipFilterPasses, texture.m_settings, 0 );
    SetTexGenMipSettings( furControl->m_MipGenFilter, furControl->m_PostMipFilter, furControl->m_MipFilterPasses, texture.m_settings, 1 );
    SetTexGenMipSettings( furControl->m_MipGenFilter, furControl->m_PostMipFilter, furControl->m_MipFilterPasses, texture.m_settings, 2 );
    SetTexGenMipSettings( furControl->m_MipGenFilter, furControl->m_PostMipFilter, furControl->m_MipFilterPasses, texture.m_settings, 3 );

    texture.m_settings.m_output_format = IG::OUTPUT_CF_DXT1; // hard-coded for now

    texture.m_processing_type = TEX_RT_COLOR_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    std::string furControlFilePath = furControl->GetFilePath();
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = furControlFilePath;
    texture.m_channels[TEXTURE_CHANNEL_BLUE] = TEXTURE_CHANNEL_BLUE;
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = furControlFilePath;
    texture.m_channels[TEXTURE_CHANNEL_GREEN] = TEXTURE_CHANNEL_GREEN;
    texture.m_channel_textures[TEXTURE_CHANNEL_RED] = furControlFilePath;
    texture.m_channels[TEXTURE_CHANNEL_RED] = TEXTURE_CHANNEL_RED;

    // No alpha
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]="";
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]=0xffffffff;

    return true;
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////

  bool MakeFoliageTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< FoliageMapAttribute > foliageMap( shader );

    if ( !foliageMap.Valid() || foliageMap->GetFilePath().empty() )
    {
      return false;
    }

    SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    SetTexRuntimeSettings( foliageMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, foliageMap->m_MipBias, texture.m_runtime );

    SetTexGenReductionRatio( foliageMap->m_ReductionRatio, texture.m_settings );

    SetTexGenMipSettings( foliageMap->m_MipGenFilter, foliageMap->m_PostMipFilter, foliageMap->m_MipFilterPasses, texture.m_settings, 0 );
    SetTexGenMipSettings( foliageMap->m_LeafThicknessMipGenFilter, foliageMap->m_LeafThicknessPostMipFilter, foliageMap->m_LeafThicknessMipFilterPasses, texture.m_settings, 1 );
    // channel 3 & 4 are set to black...

    texture.m_settings.m_output_format = ( IG::OutputColorFormat )( foliageMap->m_TexFormat );

    texture.m_processing_type = TEX_RT_COLOR_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    std::string foliageMapFilePath = foliageMap->GetFilePath();

    texture.m_channel_textures[TEXTURE_CHANNEL_RED]    = foliageMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN]  = foliageMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE]   = "";
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]  = "";

    texture.m_channels[TEXTURE_CHANNEL_RED]    = TEXTURE_CHANNEL_RED;
    texture.m_channels[TEXTURE_CHANNEL_GREEN]  = TEXTURE_CHANNEL_GREEN;
    texture.m_channels[TEXTURE_CHANNEL_BLUE]   = 0xffffffff;
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]  = 0xffffffff;

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////

  bool MakeFoliageShadowTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< FoliageShadowMapAttribute > foliageMap( shader );

    if ( !foliageMap.Valid() || foliageMap->GetFilePath().empty() )
    {
      return false;
    }

    SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    SetTexRuntimeSettings( foliageMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, foliageMap->m_MipBias, texture.m_runtime );

    SetTexGenReductionRatio( foliageMap->m_ReductionRatio, texture.m_settings );

    SetTexGenMipSettings( foliageMap->m_MipGenFilter, foliageMap->m_PostMipFilter, foliageMap->m_MipFilterPasses, texture.m_settings, 0 );
    SetTexGenMipSettings( foliageMap->m_MipGenFilter, foliageMap->m_PostMipFilter, foliageMap->m_MipFilterPasses, texture.m_settings, 1 );
    SetTexGenMipSettings( foliageMap->m_MipGenFilter, foliageMap->m_PostMipFilter, foliageMap->m_MipFilterPasses, texture.m_settings, 2 );
    SetTexGenMipSettings( foliageMap->m_MipGenFilter, foliageMap->m_PostMipFilter, foliageMap->m_MipFilterPasses, texture.m_settings, 3 );

    texture.m_settings.m_output_format = ( IG::OutputColorFormat )( foliageMap->m_TexFormat );

    texture.m_processing_type = TEX_RT_COLOR_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    std::string foliageMapFilePath = foliageMap->GetFilePath();

    texture.m_channel_textures[TEXTURE_CHANNEL_RED]    = foliageMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN]  = foliageMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE]   = foliageMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]  = foliageMapFilePath;

    texture.m_channels[TEXTURE_CHANNEL_RED]    = TEXTURE_CHANNEL_RED;
    texture.m_channels[TEXTURE_CHANNEL_GREEN]  = TEXTURE_CHANNEL_GREEN;
    texture.m_channels[TEXTURE_CHANNEL_BLUE]   = TEXTURE_CHANNEL_BLUE;
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]  = TEXTURE_CHANNEL_ALPHA;

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////

  bool MakeBRDFMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< BRDFMapAttribute > brdfMap( shader );

    if ( !brdfMap.Valid() || brdfMap->GetFilePath().empty() )
    {
      return false;
    }

    SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );

    SetTexRuntimeSettings( brdfMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, brdfMap->m_MipBias, texture.m_runtime );

    SetTexGenReductionRatio( brdfMap->m_ReductionRatio, texture.m_settings );

    SetTexGenMipSettings( brdfMap->m_MipGenFilter, brdfMap->m_PostMipFilter, brdfMap->m_MipFilterPasses, texture.m_settings, 0 );
    SetTexGenMipSettings( brdfMap->m_MipGenFilter, brdfMap->m_PostMipFilter, brdfMap->m_MipFilterPasses, texture.m_settings, 1 );
    SetTexGenMipSettings( brdfMap->m_MipGenFilter, brdfMap->m_PostMipFilter, brdfMap->m_MipFilterPasses, texture.m_settings, 2 );
    SetTexGenMipSettings( brdfMap->m_MipGenFilter, brdfMap->m_PostMipFilter, brdfMap->m_MipFilterPasses, texture.m_settings, 3 );

    texture.m_settings.m_output_format = ( IG::OutputColorFormat )( brdfMap->m_TexFormat );

    texture.m_processing_type = TEX_RT_COLOR_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );


    std::string brdfMapFilePath = brdfMap->GetFilePath();

    texture.m_channel_textures[TEXTURE_CHANNEL_RED]    = brdfMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_GREEN]  = brdfMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_BLUE]   = brdfMapFilePath;
    texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]  = brdfMapFilePath;

    texture.m_channels[TEXTURE_CHANNEL_RED]    = TEXTURE_CHANNEL_RED;
    texture.m_channels[TEXTURE_CHANNEL_GREEN]  = TEXTURE_CHANNEL_GREEN;
    texture.m_channels[TEXTURE_CHANNEL_BLUE]   = TEXTURE_CHANNEL_BLUE;
    texture.m_channels[TEXTURE_CHANNEL_ALPHA]  = TEXTURE_CHANNEL_ALPHA;

    // Max limit on size.
    if ( shader->IsCinematicShader() )
    {
      texture.m_settings.m_max_size = 2048;
    }
    else
    {
      texture.m_settings.m_max_size = 1024;
    }

    return true;
  }


  bool MakeOffScreenTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    AttributeViewer< StandardColorMapAttribute > colorMap( shader );

    if ( !colorMap.Valid() )
    {
      return false;
    }

    Asset::OffScreenShaderAsset* offScreen = Reflect::AssertCast< Asset::OffScreenShaderAsset >( shader );

    // This is a placeholder for the texture that will be created at runtime.
    // We just need to set aside the memory to use for this texture.
    ShaderProcess::SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );
    texture.m_settings.m_output_format = IG::OUTPUT_CF_ARGB8888; // Force to uncompressed
    texture.m_processing_type = TEX_RT_COLOR_MAP;
    texture.m_runtime_type = TextureSlotToRuntimeTextureType( slot );

    std::string colorMapFilePath = colorMap->GetFilePath();
    if ( !colorMapFilePath.empty() )
    {
      // there is a color map specified, use it to fill the RGB channels
      texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_BLUE]=TEXTURE_CHANNEL_BLUE;

      texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_GREEN]=TEXTURE_CHANNEL_GREEN;

      texture.m_channel_textures[TEXTURE_CHANNEL_RED] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_RED]=TEXTURE_CHANNEL_RED;

      texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA] = colorMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_ALPHA]=TEXTURE_CHANNEL_ALPHA;
    }
    else
    {
      texture.m_channel_textures[TEXTURE_CHANNEL_BLUE]="";
      texture.m_channels[TEXTURE_CHANNEL_BLUE]=0xffffffff;
      texture.m_channel_textures[TEXTURE_CHANNEL_GREEN]="";
      texture.m_channels[TEXTURE_CHANNEL_GREEN]=0xffffffff;
      texture.m_channel_textures[TEXTURE_CHANNEL_RED]="";
      texture.m_channels[TEXTURE_CHANNEL_RED]=0xffffffff;
      texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]="";
      texture.m_channels[TEXTURE_CHANNEL_ALPHA]=0xffffffff;
    }

    // Bink movies use the default texture, non-bink shaders allocate their
    // texture with the data from the color map channel.
    return offScreen->m_BinkID == 0;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  template <class T>
  bool MakeCustomMapTexture(RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    T* customMap = shader->GetAttribute<T>();
    if ( !customMap)
    {
      return false;
    }

    ShaderProcess::SetTextureSettingsToDefaults( texture.m_runtime, texture.m_settings );
    ShaderProcess::SetTexRuntimeSettings( customMap->m_TexFilter, shader->m_WrapModeU, shader->m_WrapModeV, customMap->m_MipBias, texture.m_runtime );
    ShaderProcess::SetTexGenReductionRatio( customMap->m_ReductionRatio, texture.m_settings );
    ShaderProcess::SetTexGenMipSettings( customMap->m_MipGenFilter, customMap->m_PostMipFilter,  customMap->m_MipFilterPasses, texture.m_settings, 0 );
    ShaderProcess::SetTexGenMipSettings( customMap->m_MipGenFilter, customMap->m_PostMipFilter,  customMap->m_MipFilterPasses, texture.m_settings, 1 );
    ShaderProcess::SetTexGenMipSettings( customMap->m_MipGenFilter, customMap->m_PostMipFilter,  customMap->m_MipFilterPasses, texture.m_settings, 2 );
    ShaderProcess::SetTexGenMipSettings( customMap->m_AlphaMipGenFilter,  customMap->m_AlphaPostMipFilter,  customMap->m_AlphaMipFilterPasses, texture.m_settings, 3 );

    // Max limit on size.
    if ( shader->IsCinematicShader() )
    {
      texture.m_settings.m_max_size = 2048;
    }
    else
    {
      texture.m_settings.m_max_size = 1024;
    }

    texture.m_processing_type = (ShaderTextureType)(TEX_RT_COLOR_MAP + slot);
    texture.m_runtime_type    = TextureSlotToRuntimeTextureType( slot );

    texture.m_settings.m_output_format    = ( IG::OutputColorFormat )( customMap->m_TextureFormat );
    texture.m_settings.m_generate_mips    = (customMap->m_GenerateMipMaps == GenerateMipsOptions::CUSTOM_MAP_GENERATE_MIPS_YES);
    texture.m_runtime.m_expand_range      = (customMap->m_ExpandRange     == RangeExpansionOptions::CUSTOM_MAP_EXPAND_RANGE_YES);

    //Set sRGB->Linear conversion at runtime
    texture.m_runtime.m_srgb_expand_r     = (customMap->m_IsColorMap      == CustomMapTypes::CUSTOM_MAP_IS_COLOR_MAP_YES);
    texture.m_runtime.m_srgb_expand_g     = texture.m_runtime.m_srgb_expand_r;
    texture.m_runtime.m_srgb_expand_b     = texture.m_runtime.m_srgb_expand_r;
    texture.m_runtime.m_srgb_expand_a = false;

    bool color,alpha;
    std::string customMapFilePath = customMap->GetFilePath();
    if ( !customMapFilePath.empty() )
    {
      // there is a color map specified, use it to fill the RGB channels
      texture.m_channel_textures[TEXTURE_CHANNEL_BLUE] = customMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_BLUE]=TEXTURE_CHANNEL_BLUE;

      texture.m_channel_textures[TEXTURE_CHANNEL_GREEN] = customMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_GREEN]=TEXTURE_CHANNEL_GREEN;

      texture.m_channel_textures[TEXTURE_CHANNEL_RED] = customMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_RED]=TEXTURE_CHANNEL_RED;
      color = true;

      texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA] = customMapFilePath;
      texture.m_channels[TEXTURE_CHANNEL_ALPHA]=TEXTURE_CHANNEL_ALPHA;
      alpha = true;
    }
    else
    {
      // there is no color map, use the default
      texture.m_channel_textures[TEXTURE_CHANNEL_BLUE]="";
      texture.m_channels[TEXTURE_CHANNEL_BLUE]=0xffffffff;
      texture.m_channel_textures[TEXTURE_CHANNEL_GREEN]="";
      texture.m_channels[TEXTURE_CHANNEL_GREEN]=0xffffffff;
      texture.m_channel_textures[TEXTURE_CHANNEL_RED]="";
      texture.m_channels[TEXTURE_CHANNEL_RED]=0xffffffff;
      color = false;
      texture.m_channel_textures[TEXTURE_CHANNEL_ALPHA]="";
      texture.m_channels[TEXTURE_CHANNEL_ALPHA]=0xffffffff;
      alpha = false;
    }

    if (color==false && alpha == false)
    {
      // we can use the default texture for this RT texture
      return false;
    }

    return true;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool MakeCustomMapATexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    return MakeCustomMapTexture<CustomMapAAttribute>(texture, shader, slot);
  }

  bool MakeCustomMapBTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot )
  {
    return MakeCustomMapTexture<CustomMapBAttribute>(texture, shader, slot);
  }
}
