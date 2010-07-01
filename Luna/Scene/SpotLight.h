#pragma once

#include "Light.h"
#include "Pipeline/Content/Nodes/SpotLight.h"

namespace Luna
{
  class PrimitiveCone;

  class LUNA_SCENE_API SpotLight : public Luna::Light
  {
    //
    // Members
    //
  protected:
    Luna::PrimitiveCone* m_InnerCone;
    Luna::PrimitiveCone* m_OuterCone;
    bool            m_FirstEntry;

    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( SpotLight, Luna::Light );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    SpotLight(Luna::Scene* scene);
    SpotLight(Luna::Scene* scene, Content::SpotLight* light);
    virtual ~SpotLight();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Evaluate( GraphDirection direction ) NOC_OVERRIDE;

    virtual void Initialize() NOC_OVERRIDE;
    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;
    virtual void SetScale( const Math::Scale& value );

    Luna::PrimitiveCone* GetInnerCone() const
    {
      return m_InnerCone;
    }

    Luna::PrimitiveCone* GetOuterCone() const
    {
      return m_OuterCone;
    }

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawInner( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawOuter( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawGodRayClipPlane( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static bool SetupGodRayClipPlane( IDirect3DDevice9* device, const SceneNode* object );

    virtual bool ValidatePanel(const tstring& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    float GetInnerRadius() const;
    void SetInnerRadius( float innerAngle );

    float GetOuterRadius() const;
    void SetOuterRadius( float outerAngle );

    float GetInnerConeAngle() const;
    void SetInnerConeAngle( float innerAngle );

    float GetInnerConeAngleDegrees() const;
    void SetInnerConeAngleDegrees( float innerAngle );

    float GetOuterConeAngle() const;
    void SetOuterConeAngle( float outerAngle );

    float GetOuterConeAngleDegrees() const;
    void SetOuterConeAngleDegrees( float innerAngle );

    bool GetShadowMapHiRes() const;
    void SetShadowMapHiRes( bool b );
    
    bool GetGodRayEnabled() const;
    void SetGodRayEnabled( bool b );

    float GetGodRayOpacity() const;
    void SetGodRayOpacity( float opacity );

    float GetGodRayDensity() const;
    void SetGodRayDensity(float val);

    float GetGodRayFadeNear() const;
    void SetGodRayFadeNear(float val);

    float GetGodRayFadeFar() const;
    void SetGodRayFadeFar(float val);

    float GetGodRayQuality() const;
    void SetGodRayQuality(float val);

    float GetGodRayClipPlanePhiOffset() const;
    void SetGodRayClipPlanePhiOffset(float val);

    float GetGodRayClipPlaneOffset() const;
    void SetGodRayClipPlaneOffset(float val);

    u8 GetOffsetFactor() const;
    void SetOffsetFactor( u8 factor );

    u8 GetOffsetUnits() const;
    void SetOffsetUnits( u8 units );


  };
}
