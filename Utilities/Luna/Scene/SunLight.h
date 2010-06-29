#pragma once

#include "DirectionalLight.h"
#include "Pipeline/Content/Nodes/Lights/SunLight.h"

namespace Luna
{
  class SunLight : public Luna::DirectionalLight
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( SunLight, Luna::DirectionalLight );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    SunLight(Luna::Scene* scene);
    SunLight(Luna::Scene* scene, Content::SunLight* light);

    virtual bool ValidatePanel(const tstring& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;

    float GetRedBlueShift() const;
    void SetRedBlueShift( float redBlueShift );

    float GetSaturation() const;
    void SetSaturation( float saturation );
    
    float GetHorizonHeight() const;
    void SetHorizonHeight( float horizonHeight );
   
    float GetHorizonBlur() const;
    void SetHorizonBlur( float horizonBlur );
   
    Math::Color3 GetGroundColor() const;
    void SetGroundColor( Math::Color3 color );
    
    float SunLight::GetSkyIntensity() const;
    void SetSkyIntensity( float multiplier );

    float SunLight::GetSunIntensity() const;
    void SetSunIntensity( float multiplier );

    float GetHaze() const;
    void SetHaze( float haze );
    
  };
}
