#pragma once

#include "Light.h"
#include "Pipeline/Content/Nodes/Lights/DirectionalLight.h"

namespace Luna
{
  class DirectionalLight : public Luna::Light
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::DirectionalLight, Luna::Light );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    DirectionalLight(Luna::Scene* scene);
    DirectionalLight(Luna::Scene* scene, Content::DirectionalLight* light);

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

  	// returns true if this is the main sun in a level, there can only be one
  	bool GetGlobalSun() const;
  	void SetGlobalSun( bool globalSun );

    float GetShadowSoftness() const;
    void SetShadowSoftness( float multiplier );

    int GetSoftShadowSamples() const;
    void SetSoftShadowSamples( int multiplier );
  };
}
