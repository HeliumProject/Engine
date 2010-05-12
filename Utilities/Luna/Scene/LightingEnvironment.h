#pragma once

#include "Instance.h"

#include "Content/LightingEnvironment.h"
#include "Inspect/Control.h"

namespace Luna
{
  
  class PrimitiveLocator;
  class LightingEnvironment;

  typedef Nocturnal::SmartPtr< Luna::LightingEnvironment > LightingEnvironmentPtr;

  class LightingEnvironment : public Luna::Instance
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::LightingEnvironment, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    LightingEnvironment(Luna::Scene* scene);
    LightingEnvironment(Luna::Scene* scene, Content::LightingEnvironment* environment);

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

    bool ValidateTexturePath( const Inspect::DataChangingArgs& args );

    static void CreatePanel( CreatePanelArgs& args );

    static Luna::LightingEnvironment* GetDefaultLightingEnvironment( Luna::Scene* currentScene );

    const std::string& GetTexture() const;
    void SetTexture( const std::string& texturePath );

    f32 GetIntensity() const;
    void SetIntensity( f32 intensity );

    float GetRed() const;
    void SetRed( float color );

    float GetGreen() const;
    void SetGreen( float color );

    float GetBlue() const;
    void SetBlue( float color );    
  };

  typedef Nocturnal::SmartPtr< Luna::LightingEnvironment > LightingEnvironmentPtr;
  typedef std::vector< Luna::LightingEnvironment* > V_LightingEnvironmentDumbPtr; 
}

