#include "Precompile.h"
#include "WeatherAttribute.h" 
#include "AssetClass.h"
#include "AssetManager.h"
#include "AttributeNode.h"
#include "PersistentDataFactory.h"
#include "Attribute/AttributeHandle.h"
#include "BuilderUtil/ColorPalette.h"
#include "Windows/Clipboard.h"

using namespace Luna; 

LUNA_DEFINE_TYPE( Luna::WeatherAttribute ); 

static std::string g_LastSelectedClass; 


void WeatherAttribute::InitializeType()
{
  Reflect::RegisterClass<Luna::WeatherAttribute>( "Luna::WeatherAttribute" ); 
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::WeatherAttribute>(), 
                                                  &WeatherAttribute::Create ); 
}

void WeatherAttribute::CleanupType()
{
  Reflect::UnregisterClass<Luna::WeatherAttribute>();
}

Luna::PersistentDataPtr WeatherAttribute::Create(Reflect::Element* attribute, Luna::AssetManager* manager)
{
  return new Luna::WeatherAttribute( Reflect::AssertCast< Asset::WeatherAttribute >(attribute), 
                                                   manager ); 
}

WeatherAttribute::WeatherAttribute( Asset::WeatherAttribute* attribute, Luna::AssetManager* manager)
: Luna::AttributeWrapper( attribute, manager )
{
}

WeatherAttribute::~WeatherAttribute()
{
}

void WeatherAttribute::PopulateContextMenu( ContextMenuItemSet& menu )
{
  menu.AppendSeparator();
  ContextMenuItemPtr menuItem = new ContextMenuItem( "Generate Script", "Generates the weather script and copies it to the Windows clipboard." );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &WeatherAttribute::OnGenerateScript ) );
  menu.AppendItem( menuItem );
}

void WeatherAttribute::OnGenerateScript( const ContextMenuArgsPtr& args )
{
  Asset::WeatherAttributePtr weatherAttrPtr( GetPackage<Asset::WeatherAttribute>());

  if(weatherAttrPtr != NULL)
  {
    std::stringstream script;

    script <<      "--///////////////////////////////////////////////////////////////////////////////////////////////" << std::endl;
    script <<      "function weather_script( varWeatherTransitionTimer ) prt( \"weather_script\" )" << std::endl;
    script << std::endl;

    script <<      "	local varSSAOFadeOutTransitionTimer                         = 5.0                              -- SSAO fade out timer                                                                                                                               --" << std::endl;
    script << std::endl;
    script <<      "	--ssao_set_strength(0.0, varSSAOFadeOutTransitionTimer)                                     uncomment this line to fade out SSAO while transitioning to the weather simulation                                                                --" << std::endl;
    script << std::endl;
    script <<      "	local varWeatherFogTransitionTimer                          = varWeatherTransitionTimer        -- Fog timer, if no palette entries are detected, the fog script segment will be omitted                                                             --" << std::endl;
    script <<      "	local varWeatherDropletsSizeColorTransitionTimer            = varWeatherTransitionTimer        -- Droplets Size and Color timer, if particles are not active prior to running this script this timer should be set to 0.0                           --" << std::endl;
    script <<      "	local varWeatherDropletsMotionTransitionTimer               = varWeatherTransitionTimer        -- Droplets Fall speed, Turbulence, Density and Draw distance timer                                                                                  --" << std::endl;
    script <<      "	local varWeatherLightningTransitionTimer                    = varWeatherTransitionTimer        -- Lightning                                                                                       --" << std::endl;
    script <<      "	local varWeatherWindTransitionTimer                         = varWeatherTransitionTimer        -- Wind                                                                                              --" << std::endl;
    script <<      "	local varWeatherWetnessColorTransitionTimer                 = varWeatherTransitionTimer        -- Wet-surfaces Color tint timer, if the simulation is not active prior to running this script this timer should be set to 0.0                       --" << std::endl;
    script <<      "	local varWeatherWetnessIntensityBiasTransitionTimer         = varWeatherTransitionTimer        -- Wet-surfaces Cube map intensity and bias timer                                                                                                    --" << std::endl;
    script <<      "	local varWeatherWetnessAmbientTransitionTimer               = varWeatherTransitionTimer        -- Wet-surfaces Ambient timer; this value represents how much water has seeped into the environment                                                  --" << std::endl;
    script <<      "	local varWeatherWetnessFadeTransitionTimer                  = varWeatherTransitionTimer        -- Wet-surfaces Fade distance and Fade interval timer, if the simulation is not active prior to running this script this timer should be set to 0.0  --" << std::endl;
    script <<      "	local varWeatherWetnessScrollSpeedTransitionTimer           = varWeatherTransitionTimer        -- Wet-surfaces Vertical surfaces scrolling timer                                                                                                    --" << std::endl;
    script <<      "	local varWeatherWetnessRipplesTransitionTimer               = varWeatherTransitionTimer        -- Wet-surfaces Horizontal surfaces ripples timer                                                                                                    --" << std::endl;
    script << std::endl;
    script << std::endl;
    script << std::endl;
    script <<      "	local varDropletsSize                =  " << weatherAttrPtr->m_DropletsSize                                   << std::endl;
    script <<      "	local varDropletsColorRed            =  " << Math::SRGBToLinear(f32(weatherAttrPtr->m_DropletsColor.r)/255.f) << std::endl;
    script <<      "	local varDropletsColorGreen          =  " << Math::SRGBToLinear(f32(weatherAttrPtr->m_DropletsColor.g)/255.f) << std::endl;
    script <<      "	local varDropletsColorBlue           =  " << Math::SRGBToLinear(f32(weatherAttrPtr->m_DropletsColor.b)/255.f) << std::endl;
    script <<      "	local varDropletsColorAlpha          =  " << weatherAttrPtr->m_DropletsAlpha                                  << std::endl;
    script << std::endl;
    script <<      "	weather_set_droplets_size                   (varDropletsSize      ,  varWeatherDropletsSizeColorTransitionTimer)             --   0.0  -  25.0 --" << std::endl;
    script <<      "	weather_set_droplets_color_red              (varDropletsColorRed  ,  varWeatherDropletsSizeColorTransitionTimer)             --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_droplets_color_green            (varDropletsColorGreen,  varWeatherDropletsSizeColorTransitionTimer)             --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_droplets_color_blue             (varDropletsColorBlue ,  varWeatherDropletsSizeColorTransitionTimer)             --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_droplets_color_alpha            (varDropletsColorAlpha,  varWeatherDropletsSizeColorTransitionTimer)             --   0.0  -   1.0 --" << std::endl;
    script << std::endl;
    script << std::endl;
    script <<      "	local varDropletsFallSpeed          =  " << weatherAttrPtr->m_DropletsFallSpeed                               << std::endl;        
    script <<      "	local varDropletsDistance           =  " << weatherAttrPtr->m_DropletsDistance                                << std::endl;        
    script <<      "	local varDropletsDensity            =  " << weatherAttrPtr->m_DropletsDensity                                 << std::endl;        
    script <<      "	local varDropletsTurbulence         =  " << weatherAttrPtr->m_DropletsTurbulence                              << std::endl;        
    script <<      "	local varDropletsMotionBlur         =  " << weatherAttrPtr->m_DropletsMotionBlur                              << std::endl;        
    script << std::endl;
    script <<      "	weather_set_fall_speed                      (varDropletsFallSpeed ,  varWeatherDropletsMotionTransitionTimer)                --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_distance                        (varDropletsDistance  ,  varWeatherDropletsMotionTransitionTimer)                --   5.0  -  30.0 --" << std::endl;
    script <<      "	weather_set_density                         (varDropletsDensity   ,  varWeatherDropletsMotionTransitionTimer)                --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_turbulence                      (varDropletsTurbulence,  varWeatherDropletsMotionTransitionTimer)                --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_motion_blur                     (varDropletsMotionBlur,  varWeatherDropletsMotionTransitionTimer)                --   0.0  -   1.0 --" << std::endl;
    script << std::endl;
    script << std::endl;

    script <<      "	local varWindIntensity                   =  " << weatherAttrPtr->m_WindIntensity  << std::endl;  
    script <<      "	local varWindAzimuth                     =  " << weatherAttrPtr->m_WindAzimuth    << std::endl;  

    script <<      "	local varWindGustsIntensityMin           =  " << weatherAttrPtr->m_WindGustsIntensityMin  << std::endl;  
    script <<      "	local varWindGustsIntensityMax           =  " << weatherAttrPtr->m_WindGustsIntensityMax  << std::endl;  
    script <<      "	local varWindGustsIntervalMin            =  " << weatherAttrPtr->m_WindGustsIntervalMin   << std::endl;  
    script <<      "	local varWindGustsIntervalMax            =  " << weatherAttrPtr->m_WindGustsIntervalMax   << std::endl;  
    script <<      "	local varWindGustsDurationMin            =  " << weatherAttrPtr->m_WindGustsDurationMin   << std::endl;  
    script <<      "	local varWindGustsDurationMax            =  " << weatherAttrPtr->m_WindGustsDurationMax   << std::endl;  

    script << std::endl;
    script <<      "	weather_set_wind_intensity                  (varWindIntensity,  varWeatherWindTransitionTimer)      --    0.0  -      1.0 --" << std::endl;
    script <<      "	weather_set_wind_azimuth                    (varWindAzimuth,    varWeatherWindTransitionTimer)      -- -180.0  -    180.0 --" << std::endl;
    script << std::endl;
    script <<      "	weather_set_wind_gusts_intensity            (varWindGustsIntensityMin, varWindGustsIntensityMax,  varWeatherWindTransitionTimer)      -- 0.0  -     1.0 --" << std::endl;
    script <<      "	weather_set_wind_gusts_interval             (varWindGustsIntervalMin,  varWindGustsIntervalMax,   varWeatherWindTransitionTimer)      -- 0.0  -    30.0 --" << std::endl;
    script <<      "	weather_set_wind_gusts_duration             (varWindGustsDurationMin,  varWindGustsDurationMax,   varWeatherWindTransitionTimer)      -- 0.0  -    10.0 --" << std::endl;
    script << std::endl;
    script << std::endl;

    script <<      "	local varLightningIntensityMin           =  " << weatherAttrPtr->m_LightningIntensityMin  << std::endl;  
    script <<      "	local varLightningIntensityMax           =  " << weatherAttrPtr->m_LightningIntensityMax  << std::endl;  
    script <<      "	local varLightningIntervalMin            =  " << weatherAttrPtr->m_LightningIntervalMin   << std::endl;  
    script <<      "	local varLightningIntervalMax            =  " << weatherAttrPtr->m_LightningIntervalMax   << std::endl;  
    script <<      "	local varLightningDurationMin            =  " << weatherAttrPtr->m_LightningDurationMin   << std::endl;  
    script <<      "	local varLightningDurationMax            =  " << weatherAttrPtr->m_LightningDurationMax   << std::endl;  

    script << std::endl;
    script <<      "	weather_set_lightning_intensity            (varLightningIntensityMin, varLightningIntensityMax,  varWeatherLightningTransitionTimer)   -- 0.0  -    10.0 --" << std::endl;
    script <<      "	weather_set_lightning_interval             (varLightningIntervalMin,  varLightningIntervalMax,   varWeatherLightningTransitionTimer)   -- 0.0  -    30.0 --" << std::endl;
    script <<      "	weather_set_lightning_duration             (varLightningDurationMin,  varLightningDurationMax,   varWeatherLightningTransitionTimer)   -- 0.0  -    10.0 --" << std::endl;
    script << std::endl;
    script << std::endl;

    script <<      "	local varWetnessColorRed            =  " << Math::SRGBToLinear(f32(weatherAttrPtr->m_WetnessTint.r)/255.f)                              << std::endl;  
    script <<      "	local varWetnessColorGreen          =  " << Math::SRGBToLinear(f32(weatherAttrPtr->m_WetnessTint.g)/255.f)                              << std::endl;  
    script <<      "	local varWetnessColorBlue           =  " << Math::SRGBToLinear(f32(weatherAttrPtr->m_WetnessTint.b)/255.f)                              << std::endl;  
    script << std::endl;
    script <<      "	weather_set_wetness_color_red               (varWetnessColorRed  ,  varWeatherWetnessColorTransitionTimer)                      --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_wetness_color_green             (varWetnessColorGreen,  varWeatherWetnessColorTransitionTimer)                      --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_wetness_color_blue              (varWetnessColorBlue ,  varWeatherWetnessColorTransitionTimer)                      --   0.0  -   1.0 --" << std::endl;
    script << std::endl;
    script << std::endl;
    script <<      "	local varWetnessIntensity               =  " << weatherAttrPtr->m_WetnessIntensity                               << std::endl;  
    script <<      "	local varWetnessCubeSpecType            =  " << weatherAttrPtr->m_WetnessCubeSpecType                            << std::endl;  
    script <<      "	local varWetnessCubeBias                =  " << weatherAttrPtr->m_WetnessCubeBias                                << std::endl;  
    script <<      "	local varWetnessAmbient                 =  " << weatherAttrPtr->m_WetnessAmbient                                 << std::endl;  
    script << std::endl;
    script <<      "	weather_set_wetness_intensity               (varWetnessIntensity,     varWeatherWetnessIntensityBiasTransitionTimer)             --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_wetness_cube_spec_type          (varWetnessCubeSpecType)                                                             --     0  -     3 --" << std::endl;
    script <<      "	weather_set_wetness_cube_bias               (varWetnessCubeBias,      varWeatherWetnessIntensityBiasTransitionTimer)             --  -4.0  -   0.0 --" << std::endl;
    script <<      "	weather_set_wetness_ambient                 (varWetnessAmbient,       varWeatherWetnessAmbientTransitionTimer)                   --   0.0  -   1.0 --" << std::endl;
    script << std::endl;
    script << std::endl;
    script <<      "	local varWetnessFadeFarDistance         =  " << weatherAttrPtr->m_WetnessFadeFarDistance                         << std::endl;  
    script <<      "	local varWetnessFadeInterval            =  " << weatherAttrPtr->m_WetnessFadeInterval                            << std::endl;  
    script << std::endl;
    script <<      "	weather_set_wetness_fade_far_distance       (varWetnessFadeFarDistance,  varWeatherWetnessFadeTransitionTimer)                 --  10.0  -  50.0 --" << std::endl;
    script <<      "	weather_set_wetness_fade_interval           (varWetnessFadeInterval,     varWeatherWetnessFadeTransitionTimer)                 --   1.0  -  10.0 --" << std::endl;
    script << std::endl;
    script << std::endl;
    script <<      "	local varWetnessScrollSpeed             =  " << weatherAttrPtr->m_WetnessScrollSpeed                             << std::endl;  
    script << std::endl;
    script <<      "	weather_set_wetness_scroll_speed            (varWetnessScrollSpeed,  varWeatherWetnessScrollSpeedTransitionTimer)              --   0.0  -   4.0 --" << std::endl;
    script << std::endl;
    script << std::endl;
    script <<      "	local varWetnessRipplesSplashWeight     =  " << weatherAttrPtr->m_WetnessRipplesSplashWeight                             << std::endl;  
    script <<      "	local varWetnessRipplesSplashRate       =  " << weatherAttrPtr->m_WetnessRipplesSplashRate                               << std::endl;  
    script <<      "	local varWetnessRipplesDamping          =  " << weatherAttrPtr->m_WetnessRipplesDamping                                  << std::endl;  
    script << std::endl;
    script << std::endl;
    script <<      "	weather_set_wetness_ripples_splash_weight   (varWetnessRipplesSplashWeight,  varWeatherWetnessRipplesTransitionTimer)          --   0.0  -   1.0 --" << std::endl;
    script <<      "	weather_set_wetness_ripples_splash_rate     (varWetnessRipplesSplashRate  ,  varWeatherWetnessRipplesTransitionTimer)          --   0.0  - 100.0 --" << std::endl;
    script <<      "	weather_set_wetness_ripples_damping         (varWetnessRipplesDamping     ,  varWeatherWetnessRipplesTransitionTimer)          --   0.0  -   1.0 --" << std::endl;
    script << std::endl;
    script << std::endl;

    //Fog
    {
      tuid   fogCustomPalette = weatherAttrPtr->m_FogCustomPalette;
      size_t fogWeightsSize   = weatherAttrPtr->m_FogWeights.size();
      size_t fogColorSize     = weatherAttrPtr->m_FogColors.size();

      //Check if we have any keyed values or a custom palette
      if(fogWeightsSize || fogColorSize || (fogCustomPalette != TUID::Null))
      {
        const u32 c_entriesPerRow = 16;
        const u32 c_rowCount      = 512/c_entriesPerRow;
        u32       fogData[512];

        //We have a valid palette
        ColorPalette::Palette::GeneratePalette(weatherAttrPtr->m_FogWeights, 
                                               weatherAttrPtr->m_FogColors,
                                               weatherAttrPtr->m_FogCustomPalette,
                                               fogData);
        
        for(u32 rowIndex = 0, currentFogEntry = 0; rowIndex < c_rowCount; ++rowIndex)
        {
          if(rowIndex == 0)
          {
            script <<      "	local varFogPaletteStr = string.char(";
          }  
          else
          {
            script <<      "	varFogPaletteStr = varFogPaletteStr .. string.char(";
          }

          for(u32 rowEntry = 0; rowEntry < c_entriesPerRow; ++rowEntry, ++currentFogEntry)
          {         
            const u32  a  = (fogData[currentFogEntry] >> 24) & 0xFF;
            const u32  r  = (fogData[currentFogEntry] >> 16) & 0xFF;
            const u32  g  = (fogData[currentFogEntry] >>  8) & 0xFF;
            const u32  b  = (fogData[currentFogEntry] >>  0) & 0xFF;
    
            script << a << ", " <<  r << ", " <<  g << ", " << b;

            if(rowEntry != c_entriesPerRow - 1)
            {
              script << ", "; 
            }
          }
          script << ")" << std::endl;
        }
        script << std::endl;
        script << std::endl;

        script <<      "	local fog_node                       =  pfx_create_node()  --Remove the local qualifier if global control is desired--  " << std::endl;
        script <<      "	local fog_attack_timer               =  2.0                                                                             " << std::endl;
        script <<      "	local fog_sustain_timer              = 10.0                                                                             " << std::endl;
        script <<      "	local fog_permanent_on_sustain_timer = true --by setting fog_permanent_on_sustain_timer to true we tell the engine that this fog node will permanently replace the global fog, no clean-up is necessary --" << std::endl;
        script <<      "	local fog_decay_timer                =  2.0                                                                             " << std::endl;
        script <<      "	pfx_set_node_lifetime             (fog_node, fog_attack_timer, fog_sustain_timer, fog_decay_timer)                      " << std::endl;
        script <<      "	pfx_set_node_permanent_on_sustain (fog_node, fog_permanent_on_sustain_timer)                                            " << std::endl;
        script <<      "	pfx_set_node_fog_palette          (fog_node,  varFogPaletteStr)                                                         " << std::endl;

        script << std::endl;
      }
      script <<      "end"                                                                                               << std::endl;
      script <<      "--///////////////////////////////////////////////////////////////////////////////////////////////" << std::endl;
    }

    std::string scriptString = script.str();
    std::string scriptError;
    Windows::CopyToClipboard(NULL, scriptString, scriptError);
  }
}