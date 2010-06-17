#pragma once

#include "Light.h"
#include "Pipeline/Content/Nodes/Lights/PortalLight.h"

namespace Luna
{
  class PrimitiveCube;

  class LUNA_SCENE_API PortalLight : public Luna::Light
  {
    //
    // Members
    //
  protected:

    Luna::PrimitiveCube* m_Cube;

    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::PortalLight, Luna::Light );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    PortalLight(Luna::Scene* scene);
    PortalLight(Luna::Scene* scene, Content::PortalLight* light);
    virtual ~PortalLight();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Initialize() NOC_OVERRIDE;
    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;
    virtual void SetScale( const Math::Scale& value );

     Luna::PrimitiveCube* GetCube() const
    {
      return m_Cube;
    }

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

  };
}
