#pragma once

#include "Light.h"
#include "Content/PointLight.h"

namespace Luna
{
  class PrimitiveSphere;

  class LUNA_SCENE_API PointLight : public Luna::Light
  {
    //
    // Members
    //
  protected:
    Luna::PrimitiveSphere* m_InnerSphere;
    Luna::PrimitiveSphere* m_OuterSphere;

    bool m_FirstEntry;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::PointLight, Luna::Light );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    PointLight(Luna::Scene* scene);
    PointLight(Luna::Scene* scene, Content::PointLight* light);
    virtual ~PointLight();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual void Evaluate( GraphDirection direction ) NOC_OVERRIDE;

    virtual void Initialize() NOC_OVERRIDE;
    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;
    virtual void SetScale( const Math::Scale& value );

    Luna::PrimitiveSphere* GetInnerSphere() const
    {
      return m_InnerSphere;
    }

    Luna::PrimitiveSphere* GetOuterSphere() const
    {
      return m_OuterSphere;
    }

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

    float GetInnerRadius() const;
    void SetInnerRadius( float radius );

    float GetOuterRadius() const;
    void SetOuterRadius( float radius );    

  };
}
