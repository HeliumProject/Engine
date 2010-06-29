#pragma once

#include "Light.h"
#include "Pipeline/Content/Nodes/Lights/AmbientLight.h"

namespace Luna
{
  class AmbientLight : public Luna::Light
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::AmbientLight, Luna::Light );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    AmbientLight(Luna::Scene* scene);
    AmbientLight(Luna::Scene* scene, Content::AmbientLight* light);

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;

    virtual bool ValidatePanel(const tstring& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    // UI Getter/Setters
    Math::Color3 GetOcclusionColor() const;
    void   SetOcclusionColor( Math::Color3 color );
    float  GetOcclusionIntensity() const;
    void   SetOcclusionIntensity( float intensity );
    f32    GetSampleDistance() const;
    void   SetSampleDistance( f32 distance );
    u32    GetSamples() const;
    void   SetSamples( u32 samples );
    bool   GetDoOcclusion() const;
    void   SetDoOcclusion( bool occlusion );
    f32    GetSpread() const;
    void   SetSpread( float spread );
    bool   GetSampleEnvironment() const;
    void   SetSampleEnvironment( bool sample );
    

  };
}
